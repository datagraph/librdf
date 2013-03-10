/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer.h"

#include "rdf++/quad.h"
#include "rdf++/raptor.h"
#include "rdf++/term.h"
#include "rdf++/triple.h"

#include <cassert>   /* for assert() */
#include <new>       /* for std::bad_alloc */
#include <stdexcept> /* for std::invalid_argument, std::runtime_error */

#include <raptor2/raptor2.h> /* for raptor_*() */

using namespace rdf;

class writer::implementation : private boost::noncopyable {
  public:
    implementation(
      std::ostream& stream,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    ~implementation();

    void begin();

    void finish();

    void flush();

    void write_triple(const triple& triple);

    void write_quad(const quad& quad);

  private:
    std::ostream& _stream;
    const std::string _content_type;
    const std::string _charset;
    raptor_world* _world = nullptr;
    raptor_uri* _base_uri = nullptr;
    raptor_iostream* _iostream = nullptr;
    raptor_serializer* _serializer = nullptr;
    raptor_statement* _statement = nullptr;

    raptor_term* make_raptor_term(const rdf::term_position pos, const rdf::term& term);

    void write_statement();
};

writer::implementation::implementation(std::ostream& stream,
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

  _base_uri = raptor_new_uri(_world, (const unsigned char*)base_uri.c_str());
  if (_base_uri == nullptr) {
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  _iostream = raptor_new_iostream_from_std_ostream(_world, &_stream);
  if (_iostream == nullptr) {
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  std::string serializer_name("nquads"); // TODO
/*
  for (unsigned int i = 0; i < rdf_format_count; i++) {
    const rdf_format_t* const format_info = &rdf_format_info[i];
    if (str_equal(content_type, format_info->content_type)) {
      serializer_name = format_info->serializer_name;
      break;
    }
  }
*/
  assert(!serializer_name.empty()); /* already validated by rdf_format_supported() */

  _serializer = raptor_new_serializer(_world, serializer_name.c_str());
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

writer::implementation::~implementation() {
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
writer::implementation::begin() {
  const int rc = raptor_serializer_start_to_iostream(_serializer, _base_uri, _iostream);
  if (rc != 0) {
    throw std::runtime_error("raptor_serializer_start_to_iostream() failed");
  }
}

void
writer::implementation::finish() {
  const int rc = raptor_serializer_serialize_end(_serializer);
  if (rc != 0) {
    throw std::runtime_error("raptor_serializer_serialize_end() failed");
  }
}

void
writer::implementation::flush() {
  const int rc = raptor_serializer_flush(_serializer);
  if (rc != 0) {
    throw std::runtime_error("raptor_serializer_flush() failed");
  }
}

raptor_term*
writer::implementation::make_raptor_term(const rdf::term_position pos,
                                         const rdf::term& term) {
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
      const auto literal = dynamic_cast<const rdf::plain_literal&>(term);
      return raptor_new_term_from_literal(_world,
        reinterpret_cast<const unsigned char*>(literal.string.c_str()),
        nullptr,
        literal.language_tag.empty() ? nullptr :
          reinterpret_cast<const unsigned char*>(literal.language_tag.c_str()));
    }

    case rdf::term_type::typed_literal: {
      assert(pos == rdf::term_position::object);
      const auto literal = dynamic_cast<const rdf::typed_literal&>(term);
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
writer::implementation::write_statement() {
  const int rc = raptor_serializer_serialize_statement(_serializer, _statement);
  if (rc != 0) {
    throw std::runtime_error("raptor_serializer_serialize_statement() failed");
  }
}

void
writer::implementation::write_triple(const triple& triple) {
  raptor_statement_clear(_statement);

  _statement->subject   = make_raptor_term(rdf::term_position::subject,   triple.subject);
  _statement->predicate = make_raptor_term(rdf::term_position::predicate, triple.predicate);
  _statement->object    = make_raptor_term(rdf::term_position::object,    triple.object);

  write_statement();

  raptor_statement_clear(_statement);
}

void
writer::implementation::write_quad(const quad& quad) {
  raptor_statement_clear(_statement);

  _statement->subject   = make_raptor_term(rdf::term_position::subject,   quad.subject);
  _statement->predicate = make_raptor_term(rdf::term_position::predicate, quad.predicate);
  _statement->object    = make_raptor_term(rdf::term_position::object,    quad.object);
  _statement->graph     = make_raptor_term(rdf::term_position::context,   quad.context);

  write_statement();

  raptor_statement_clear(_statement);
}

writer::writer(std::ostream& stream,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(new writer::implementation(stream, content_type, charset, base_uri)) {}

writer::~writer() = default;

void
writer::define_prefix(const std::string& prefix,
                      const std::string& uri_string) {
  (void)prefix, (void)uri_string; // TODO
}

void
writer::begin() {
  assert(_implementation != nullptr);
  _implementation->begin();
}

void
writer::finish() {
  assert(_implementation != nullptr);
  _implementation->finish();
}

void
writer::flush() {
  assert(_implementation != nullptr);
  _implementation->flush();
}

void
writer::write_triple(const triple& triple) {
  assert(_implementation != nullptr);
  _implementation->write_triple(triple);
}

void
writer::write_quad(const quad& quad) {
  assert(_implementation != nullptr);
  _implementation->write_quad(quad);
}
