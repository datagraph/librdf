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

  private:
    std::istream& _stream;
    const std::string _content_type;
    const std::string _charset;
    raptor_world* _world = nullptr;
    raptor_uri* _base_uri = nullptr;
    raptor_parser* _parser = nullptr;
    raptor_statement* _statement = nullptr;
};

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

  _base_uri = raptor_new_uri(_world, (const unsigned char*)base_uri.c_str());
  if (_base_uri == nullptr) {
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  std::string parser_name("nquads"); // TODO
  assert(!parser_name.empty());

  _parser = raptor_new_parser(_world, parser_name.c_str());
  if (_parser == nullptr) {
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  _statement = raptor_new_statement(_world);
  if (_statement == nullptr) {
    raptor_free_parser(_parser), _parser = nullptr;
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

  if (_base_uri != nullptr) {
    raptor_free_uri(_base_uri), _base_uri = nullptr;
  }

  if (_world != nullptr) {
    raptor_free_world(_world), _world = nullptr;
  }
}

reader::reader(std::istream& stream,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(new reader::implementation(
      stream, content_type, charset, base_uri)) {}

reader::~reader() = default;

std::unique_ptr<triple>
reader::read_triple() {
  return std::move(std::unique_ptr<triple>(nullptr));
}

std::unique_ptr<quad>
reader::read_quad() {
  return std::move(std::unique_ptr<quad>(nullptr));
}
