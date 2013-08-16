/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer/raptor.h"

#include "rdf++/format.h"
#include "rdf++/quad.h"
#include "rdf++/raptor.h"
#include "rdf++/term.h"
#include "rdf++/triple.h"

#include <cassert>    /* for assert() */
#include <functional> /* for std::function */
#include <new>        /* for std::bad_alloc */
#include <stdexcept>  /* for std::invalid_argument, std::runtime_error */

#include <raptor2/raptor2.h> /* for raptor_*() */

namespace {
  struct implementation : public rdf::writer::implementation {
    implementation(FILE* stream,
      const char* content_type,
      const char* charset,
      const char* base_uri);
    virtual ~implementation() noexcept override;
    virtual void configure(const char* key, const char* value) override;
    virtual void begin() override;
    virtual void finish() override;
    virtual void write_triple(const rdf::triple& triple) override;
    virtual void write_quad(const rdf::quad& quad) override;
    virtual void write_comment(const char* comment) override;
    virtual void flush() override;

  public:
    static void log_callback(void* user_data, raptor_log_message* message);

  private:
    const std::string _content_type;
    const std::string _charset;
    raptor_world* _world = nullptr;
    raptor_uri* _base_uri = nullptr;
    raptor_iostream* _iostream = nullptr;
    raptor_serializer* _serializer = nullptr;
    raptor_statement* _statement = nullptr;

    void initialize(const char* base_uri, std::function<raptor_iostream* ()> make_raptor_iostream);

    raptor_term* make_raptor_term(const rdf::term_position pos, const rdf::term& term);

    void write_statement();
  };
}

rdf::writer::implementation*
rdf_writer_for_raptor(FILE* const stream,
                      const char* const content_type,
                      const char* const charset,
                      const char* const base_uri) {
  return new implementation(stream, content_type, charset, base_uri);
}

/**
 * @see http://librdf.org/raptor/api/raptor2-section-general.html#raptor-log-handler
 */
void
implementation::log_callback(void* const user_data,
                             raptor_log_message* const message) {
  //auto writer_impl = reinterpret_cast<implementation*>(user_data);
  //assert(writer_impl != nullptr);
  assert(message != nullptr);

  fprintf(stderr, "implementation::log_callback(%p, %p): %s\n", user_data, message, message->text);
}

implementation::implementation(FILE* const stream,
                               const char* const content_type,
                               const char* const charset,
                               const char* const base_uri)
  : _content_type(content_type),
    _charset(charset) {
  assert(stream != nullptr);

  initialize(base_uri, [this, &stream]() -> raptor_iostream* {
    return raptor_new_iostream_to_file_handle(_world, stream);
  });
}

void
implementation::initialize(const char* const base_uri,
                           std::function<raptor_iostream* ()> make_raptor_iostream) {
  const rdf::format* const format = rdf::format::find_for_content_type(_content_type);
  assert(format != nullptr);

  const char* const serializer_name = format->serializer_name;
  assert(serializer_name != nullptr);

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

  _iostream = make_raptor_iostream();
  if (_iostream == nullptr) {
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  _serializer = raptor_new_serializer(_world, serializer_name);
  if (_serializer == nullptr) {
    raptor_free_iostream(_iostream), _iostream = nullptr;
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  _statement = raptor_new_statement(_world);
  if (_statement == nullptr) {
    raptor_free_serializer(_serializer), _serializer = nullptr;
    raptor_free_iostream(_iostream), _iostream = nullptr;
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }
}

implementation::~implementation() noexcept {
  if (_statement != nullptr) {
    raptor_free_statement(_statement), _statement = nullptr;
  }

  if (_serializer != nullptr) {
    raptor_free_serializer(_serializer), _serializer = nullptr;
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
implementation::configure(const char* const key,
                          const char* const value) {
  (void)key, (void)value; // TODO
}

void
implementation::begin() {
  const int rc = raptor_serializer_start_to_iostream(_serializer, _base_uri, _iostream);
  if (rc != 0) {
    throw std::runtime_error("raptor_serializer_start_to_iostream() failed");
  }
}

void
implementation::finish() {
  const int rc = raptor_serializer_serialize_end(_serializer);
  if (rc != 0) {
    throw std::runtime_error("raptor_serializer_serialize_end() failed");
  }
}

raptor_term*
implementation::make_raptor_term(const rdf::term_position pos,
                                 const rdf::term& term) {
  (void)pos;
  switch (term.type) {
    case rdf::term_type::none: { /* the default context only */
      assert(pos == rdf::term_position::context);
      return nullptr;
    }

    case rdf::term_type::uri_reference: {
      return raptor_new_term_from_uri_string(_world,
        reinterpret_cast<const unsigned char*>(term.string.c_str()));
    }

    case rdf::term_type::blank_node: {
      assert(pos == rdf::term_position::subject || pos == rdf::term_position::object);
      return raptor_new_term_from_blank(_world,
        reinterpret_cast<const unsigned char*>(term.string.c_str()));
    }

    case rdf::term_type::plain_literal: {
      assert(pos == rdf::term_position::object);
      const rdf::plain_literal& literal = dynamic_cast<const rdf::plain_literal&>(term);
      return raptor_new_term_from_literal(_world,
        reinterpret_cast<const unsigned char*>(literal.string.c_str()),
        nullptr,
        literal.language_tag.empty() ? nullptr :
          reinterpret_cast<const unsigned char*>(literal.language_tag.c_str()));
    }

    case rdf::term_type::typed_literal: {
      assert(pos == rdf::term_position::object);
      const rdf::typed_literal& literal = dynamic_cast<const rdf::typed_literal&>(term);
      return raptor_new_term_from_literal(_world,
        reinterpret_cast<const unsigned char*>(literal.string.c_str()),
        raptor_new_uri(_world, reinterpret_cast<const unsigned char*>(literal.datatype_uri.c_str())),
        nullptr);
    }
  }
  assert(false); /* should never get here */
  return nullptr;
}

void
implementation::write_statement() {
  const int rc = raptor_serializer_serialize_statement(_serializer, _statement);
  if (rc != 0) {
    throw std::runtime_error("raptor_serializer_serialize_statement() failed");
  }
}

void
implementation::write_triple(const rdf::triple& triple) {
  raptor_statement_clear(_statement);

  _statement->subject   = make_raptor_term(rdf::term_position::subject,   *triple.subject);
  _statement->predicate = make_raptor_term(rdf::term_position::predicate, *triple.predicate);
  _statement->object    = make_raptor_term(rdf::term_position::object,    *triple.object);

  write_statement();

  raptor_statement_clear(_statement);

  _count++;
}

void
implementation::write_quad(const rdf::quad& quad) {
  raptor_statement_clear(_statement);

  _statement->subject   = make_raptor_term(rdf::term_position::subject,   *quad.subject);
  _statement->predicate = make_raptor_term(rdf::term_position::predicate, *quad.predicate);
  _statement->object    = make_raptor_term(rdf::term_position::object,    *quad.object);
  if (quad.context != nullptr) {
    _statement->graph   = make_raptor_term(rdf::term_position::context,   *quad.context);
  }

  write_statement();

  raptor_statement_clear(_statement);

  _count++;
}

void
implementation::write_comment(const char* const comment) {
  (void)comment; // TODO
}

void
implementation::flush() {
  const int rc = raptor_serializer_flush(_serializer);
  if (rc != 0) {
    throw std::runtime_error("raptor_serializer_flush() failed");
  }
}
