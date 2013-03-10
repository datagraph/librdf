/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer.h"

#include <cassert>   /* for assert() */
#include <new>       /* for std::bad_alloc */
#include <stdexcept> /* for std::invalid_argument, std::runtime_error */

#include <raptor2/raptor2.h> /* for raptor_*() */

using namespace rdf;

class writer::implementation : private boost::noncopyable {
  public:
    implementation(
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    ~implementation();

  private:
    const std::string _content_type;
    const std::string _charset;
    raptor_world* _world = nullptr;
    raptor_uri* _base_uri = nullptr;
    raptor_serializer* _serializer = nullptr;
    raptor_statement* _statement = nullptr;
};

writer::implementation::implementation(const std::string& content_type,
                                       const std::string& charset,
                                       const std::string& base_uri)
  : _content_type(content_type),
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
    raptor_free_uri(_base_uri), _base_uri = nullptr;
    raptor_free_world(_world), _world = nullptr;
    throw std::bad_alloc(); /* out of memory */
  }

  _statement = raptor_new_statement(_world);
  if (_statement == nullptr) {
    raptor_free_serializer(_serializer), _serializer = nullptr;
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

  if (_base_uri != nullptr) {
    raptor_free_uri(_base_uri), _base_uri = nullptr;
  }

  if (_world != nullptr) {
    raptor_free_world(_world), _world = nullptr;
  }
}

writer::writer(const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(new writer::implementation(content_type, charset, base_uri)) {}

writer::~writer() = default;
