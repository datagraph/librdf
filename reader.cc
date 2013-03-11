/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/reader.h"

#include "rdf++/quad.h"
#include "rdf++/raptor.h"
#include "rdf++/term.h"
#include "rdf++/triple.h"

#include <cassert>   /* for assert() */
#include <iostream>  /* for std::cerr, std::endl */
#include <new>       /* for std::bad_alloc */
#include <stdexcept> /* for std::invalid_argument, std::runtime_error */

#include <raptor2/raptor2.h> /* for raptor_*() */

using namespace rdf;

class reader::implementation : private boost::noncopyable {
  public:
    implementation(
      std::istream& stream,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    ~implementation();

    void read();

    void read_triples(std::function<void (rdf::triple*)> callback);

    void read_quads(std::function<void (rdf::quad*)> callback);

    void abort();

    static void log_callback(void* user_data, raptor_log_message* message);

    static void statement_callback(void* user_data, raptor_statement* statement);

  private:
    std::istream& _stream;
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

/**
 * @see http://librdf.org/raptor/api/raptor2-section-general.html#raptor-log-handler
 */
void
reader::implementation::log_callback(void* const user_data,
                                     raptor_log_message* const message) {
  auto reader_impl = reinterpret_cast<reader::implementation*>(user_data);
  assert(reader_impl != nullptr);
  assert(message != nullptr);

  fprintf(stderr, "reader::implementation::log_callback(%p, %p): %s\n", user_data, message, message->text);
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
reader::implementation::statement_callback(void* const user_data,
                                           raptor_statement* const statement) {
  auto reader_impl = reinterpret_cast<reader::implementation*>(user_data);
  assert(reader_impl != nullptr);
  assert(statement != nullptr);

#if 0
  fprintf(stderr, "reader::implementation::statement_callback(%p, %p)\n", user_data, statement);
#endif

  rdf::term* const subject   = copy_raptor_term(rdf::term_position::subject,   statement->subject);
  rdf::term* const predicate = copy_raptor_term(rdf::term_position::predicate, statement->predicate);
  rdf::term* const object    = copy_raptor_term(rdf::term_position::object,    statement->object);

  if (reader_impl->_quad_callback) {
    rdf::term* const context = copy_raptor_term(rdf::term_position::context, statement->graph);
    rdf::quad* quad = new rdf::quad(subject, predicate, object, context);

    reader_impl->_quad_callback(quad);
  }
  else if (reader_impl->_triple_callback) {
    rdf::triple* triple = new rdf::triple(subject, predicate, object);

    reader_impl->_triple_callback(triple);
  }
}

reader::implementation::implementation(std::istream& stream,
                                       const std::string& content_type,
                                       const std::string& charset,
                                       const std::string& base_uri)
  : _stream(stream),
    _content_type(content_type),
    _charset(charset) {

  _world = raptor_new_world();
  if (_world == nullptr) {
    throw std::bad_alloc(); /* out of memory */
  }
  raptor_world_set_log_handler(_world, this, reader::implementation::log_callback);
  raptor_world_open(_world);

  _base_uri = raptor_new_uri(_world, (const unsigned char*)base_uri.c_str());
  if (_base_uri == nullptr) {
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  _iostream = raptor_new_iostream_from_std_istream(_world, &_stream);
  if (_iostream == nullptr) {
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  std::string parser_name("nquads"); // TODO
  assert(!parser_name.empty());

  _parser = raptor_new_parser(_world, parser_name.c_str());
  if (_parser == nullptr) {
    raptor_free_iostream(_iostream), _iostream = nullptr;
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }
  raptor_parser_set_statement_handler(_parser, this, reader::implementation::statement_callback);

  _statement = raptor_new_statement(_world);
  if (_statement == nullptr) {
    raptor_free_parser(_parser), _parser = nullptr;
    raptor_free_iostream(_iostream), _iostream = nullptr;
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }
}

reader::implementation::~implementation() {
  if (_statement != nullptr) {
    raptor_free_statement(_statement), _statement = nullptr;
  }

  if (_parser != nullptr) {
    raptor_free_parser(_parser), _parser = nullptr;
  }

  if (_iostream != nullptr) {
    raptor_free_iostream(_iostream), _iostream = nullptr;
  }

  if (_base_uri != nullptr) {
    raptor_free_uri(_base_uri), _base_uri = nullptr;
  }

  if (_world != nullptr) {
    raptor_free_world(_world), _world = nullptr;
  }
}

void
reader::implementation::read() {
  const int rc = raptor_parser_parse_iostream(_parser, _iostream, _base_uri);
  if (rc != 0) {
    throw std::runtime_error("raptor_parser_parse_iostream() failed");
  }
}

void
reader::implementation::read_triples(std::function<void (rdf::triple*)> callback) {
  _triple_callback = callback;
  _quad_callback   = nullptr;
  read();
}

void
reader::implementation::read_quads(std::function<void (rdf::quad*)> callback) {
  _triple_callback = nullptr;
  _quad_callback   = callback;
  read();
}

void
reader::implementation::abort() {
  raptor_parser_parse_abort(_parser);
}

reader::reader(std::istream& stream,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(new reader::implementation(
      stream, content_type, charset, base_uri)) {}

reader::~reader() = default;

void
reader::read_triples(std::function<void (rdf::triple*)> callback) {
  assert(_implementation != nullptr);
  _implementation->read_triples(callback);
}

void
reader::read_quads(std::function<void (rdf::quad*)> callback) {
  assert(_implementation != nullptr);
  _implementation->read_quads(callback);
}

void
reader::abort() {
  assert(_implementation != nullptr);
  _implementation->abort();
}
