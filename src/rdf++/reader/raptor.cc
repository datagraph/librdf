/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/reader/raptor.h"

#include "rdf++/format.h"
#include "rdf++/quad.h"
#include "rdf++/raptor.h"
#include "rdf++/term.h"
#include "rdf++/triple.h"

#include <cassert>    /* for assert() */
#include <functional> /* for std::function */
#include <iostream>   /* for std::cerr, std::endl */
#include <new>        /* for std::bad_alloc */
#include <stdexcept>  /* for std::invalid_argument, std::runtime_error */

#include <raptor2/raptor2.h> /* for raptor_*() */

namespace {
  struct implementation : public rdf::reader::implementation {
    implementation(FILE* stream,
      const char* content_type,
      const char* charset,
      const char* base_uri);
    virtual ~implementation() noexcept override;
    virtual void read_triples(std::function<void (rdf::triple*)> callback) override;
    virtual void read_quads(std::function<void (rdf::quad*)> callback) override;
    virtual void abort() override;

  public:
    void read();
    static void statement_callback(void* user_data, raptor_statement* statement);
    static void log_callback(void* user_data, raptor_log_message* message);

  private:
    const std::string _content_type;
    const std::string _charset;
    raptor_world* _world = nullptr;
    raptor_uri* _base_uri = nullptr;
    raptor_iostream* _iostream = nullptr;
    raptor_parser* _parser = nullptr;
    raptor_statement* _statement = nullptr;
    std::function<void (rdf::triple*)> _triple_callback;
    std::function<void (rdf::quad*)> _quad_callback;
  };
}

rdf::reader::implementation*
rdf_reader_for_raptor(FILE* const stream,
                      const char* const content_type,
                      const char* const charset,
                      const char* const base_uri) {
  return new implementation(stream, content_type, charset, base_uri);
}

implementation::implementation(FILE* const stream,
                               const char* const content_type,
                               const char* const charset,
                               const char* const base_uri)
  : _content_type(content_type),
    _charset(charset) {
  assert(stream != nullptr);

  const rdf::format* const format = rdf::format::find_for_content_type(_content_type);
  assert(format != nullptr);

  const char* const parser_name = format->parser_name;
  assert(parser_name != nullptr);

  _world = raptor_new_world();
  if (_world == nullptr) {
    throw std::bad_alloc(); /* out of memory */
  }
  raptor_world_set_log_handler(_world, this, implementation::log_callback);
  raptor_world_open(_world);

  _base_uri = raptor_new_uri(_world, (const unsigned char*)base_uri);
  if (_base_uri == nullptr) {
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  _iostream = raptor_new_iostream_from_file_handle(_world, stream);
  if (_iostream == nullptr) {
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  _parser = raptor_new_parser(_world, parser_name);
  if (_parser == nullptr) {
    raptor_free_iostream(_iostream), _iostream = nullptr;
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }
  raptor_parser_set_statement_handler(_parser, this, implementation::statement_callback);

  _statement = raptor_new_statement(_world);
  if (_statement == nullptr) {
    raptor_free_parser(_parser), _parser = nullptr;
    raptor_free_iostream(_iostream), _iostream = nullptr;
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }
}

implementation::~implementation() noexcept {
  if (_statement != nullptr) {
    raptor_free_statement(_statement);
    _statement = nullptr;
  }

  if (_parser != nullptr) {
    raptor_free_parser(_parser);
    _parser = nullptr;
  }

  if (_iostream != nullptr) {
    raptor_free_iostream(_iostream);
    _iostream = nullptr;
  }

  if (_base_uri != nullptr) {
    raptor_free_uri(_base_uri);
    _base_uri = nullptr;
  }

  if (_world != nullptr) {
    raptor_free_world(_world);
    _world = nullptr;
  }
}

void
implementation::read_triples(std::function<void (rdf::triple*)> callback) {
  _triple_callback = callback;
  _quad_callback   = nullptr;
  read();
}

void
implementation::read_quads(std::function<void (rdf::quad*)> callback) {
  _triple_callback = nullptr;
  _quad_callback   = callback;
  read();
}

void
implementation::abort() {
  raptor_parser_parse_abort(_parser);
}

void
implementation::read() {
  const int rc = raptor_parser_parse_iostream(_parser, _iostream, _base_uri);
  if (rc != 0) {
    throw rdf::reader_error("raptor_parser_parse_iostream() failed");
  }
}

static rdf::term*
copy_raptor_term(const rdf::term_position pos,
                 const raptor_term* const input) {
  if (pos == rdf::term_position::context && input == nullptr) {
    return nullptr; /* special case for the default context */
  }

  assert(input != nullptr);

  rdf::term* term = nullptr;

  switch (input->type) {
    case RAPTOR_TERM_TYPE_BLANK: {
      const auto node_label = reinterpret_cast<const char*>(
        input->value.blank.string);
      term = new rdf::blank_node(node_label);
      break;
    }

    case RAPTOR_TERM_TYPE_URI: {
      const auto uri_string = reinterpret_cast<const char*>(
        raptor_uri_as_string(input->value.uri));
      term = new rdf::uri_reference(uri_string);
      break;
    }

    case RAPTOR_TERM_TYPE_LITERAL: {
      const auto lexical_form = reinterpret_cast<const char*>(
        input->value.literal.string);
      if (input->value.literal.datatype != nullptr) {
        const auto datatype_uri = reinterpret_cast<const char*>(
          raptor_uri_as_string(input->value.literal.datatype));
        term = new rdf::typed_literal(lexical_form, datatype_uri);
      }
      else if (input->value.literal.language != nullptr) {
        const auto language_tag = reinterpret_cast<const char*>(
          input->value.literal.language);
        term = new rdf::plain_literal(lexical_form, language_tag);
      }
      else {
        term = new rdf::plain_literal(lexical_form);
      }
      break;
    }

    case RAPTOR_TERM_TYPE_UNKNOWN:
    default: {
      assert(false); /* should never get here */
      break;
    }
  }

  assert(term != nullptr);
  return term;
}

void
implementation::statement_callback(void* const user_data,
                                   raptor_statement* const statement) {
  auto reader_impl = reinterpret_cast<implementation*>(user_data);
  assert(reader_impl != nullptr);
  assert(statement != nullptr);

#if 0
  fprintf(stderr, "implementation::statement_callback(%p, %p)\n", user_data, statement);
#endif

  rdf::term* const subject   = copy_raptor_term(rdf::term_position::subject,   statement->subject);
  rdf::term* const predicate = copy_raptor_term(rdf::term_position::predicate, statement->predicate);
  rdf::term* const object    = copy_raptor_term(rdf::term_position::object,    statement->object);

  if (reader_impl->_quad_callback) {
    rdf::term* const context = copy_raptor_term(rdf::term_position::context, statement->graph);
    rdf::quad* const quad = new rdf::quad(subject, predicate, object, context);

    reader_impl->_quad_callback(quad);
  }
  else if (reader_impl->_triple_callback) {
    rdf::triple* const triple = new rdf::triple(subject, predicate, object);

    reader_impl->_triple_callback(triple);
  }
}

/**
 * @see http://librdf.org/raptor/api/raptor2-section-general.html#raptor-log-handler
 */
void
implementation::log_callback(void* const user_data,
                             raptor_log_message* const message) {
  //auto reader_impl = reinterpret_cast<implementation*>(user_data);
  //assert(reader_impl != nullptr);
  assert(message != nullptr);

#if 1
  fprintf(stderr, "rdf::reader::implementation::log_callback(%p, %p): message=%s locator=%d:%d\n",
    user_data, message, message->text, message->locator->line, message->locator->column);
#endif
}
