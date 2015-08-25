/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "reader.h"
#include "reader/impl.h"

#ifndef DISABLE_JSONLD
#include "reader/jsonld.h"
#endif

#ifndef DISABLE_NQUADS
#include "reader/nquads.h"
#endif

#ifndef DISABLE_RDFJSON
#include "reader/rdfjson.h"
#endif

#ifndef DISABLE_TRIX
#include "reader/trix.h"
#endif

#ifdef HAVE_LIBRAPTOR2
#include "raptor.h"
#include "reader/raptor.h"
#endif

#ifdef HAVE_LIBSERD
#include "reader/serd.h"
#endif

#include "format.h"
#include "quad.h"
#include "term.h"
#include "triple.h"

#include <cassert>    /* for assert() */
#include <cstdio>     /* for std::fopen() */
#include <cstring>    /* for std::strcmp(), std::strerror() */
#include <functional> /* for std::function */
#include <stdexcept>  /* for std::invalid_argument, std::runtime_error */
#include <cerrno>     /* for errno */
#include <cstdio>     /* for std::snprintf() */

using namespace rdf;

static rdf::reader::implementation*
rdf_reader_for(FILE* const stream,
               const char* const content_type = nullptr,
               const char* const charset = nullptr,
               const char* const base_uri = nullptr) {

#ifdef HAVE_LIBRAPTOR2
  /* Only the Raptor implementation supports content autodetection at the moment: */
  if (!content_type) {
    return rdf_reader_for_raptor(stream, content_type, charset, base_uri);
  }
#endif

  const format* const format = format::find_for_content_type(content_type);
  if (!format) {
    return nullptr; /* unknown content type */
  }

#ifndef DISABLE_JSONLD
  if (std::strcmp("jsonld", format->module_name) == 0) {
    return rdf_reader_for_jsonld(stream, content_type, charset, base_uri);
  }
#endif

#ifndef DISABLE_NQUADS
  if (std::strcmp("nquads", format->module_name) == 0) {
    return rdf_reader_for_nquads(stream, content_type, charset, base_uri);
  }
#endif

#ifndef DISABLE_RDFJSON
  if (std::strcmp("rdfjson", format->module_name) == 0) {
    return rdf_reader_for_rdfjson(stream, content_type, charset, base_uri);
  }
#endif

#ifndef DISABLE_TRIX
  if (std::strcmp("trix", format->module_name) == 0) {
    return rdf_reader_for_trix(stream, content_type, charset, base_uri);
  }
#endif

#ifdef HAVE_LIBRAPTOR2
  if (std::strcmp("raptor", format->module_name) == 0) {
    return rdf_reader_for_raptor(stream, content_type, charset, base_uri);
  }
#endif

#ifdef HAVE_LIBSERD
  if (std::strcmp("serd", format->module_name) == 0) {
    return rdf_reader_for_serd(stream, content_type, charset, base_uri);
  }
#endif

  (void)stream, (void)charset, (void)base_uri;
  assert(false && "inconsistent content type definitions");
  return nullptr; /* no implementation available */
}

reader::reader(const std::string& file_path,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation{nullptr} {

  FILE* const stream = std::fopen(file_path.c_str(), "r");

  if (!stream) {
    char error_message[1024];
    std::snprintf(error_message, sizeof(error_message),
      "failed to open '%s': %s", file_path.c_str(), std::strerror(errno));
    throw std::invalid_argument{error_message};
  }

  _implementation.reset(rdf_reader_for(stream, content_type.c_str(), charset.c_str(), base_uri.c_str()));

  if (!_implementation) {
    char error_message[1024];
    std::snprintf(error_message, sizeof(error_message),
      "unknown content type: %s", content_type.c_str());
    throw std::invalid_argument{error_message};
  }
}

reader::reader(std::istream& /*stream*/,
               const std::string& /*content_type*/,
               const std::string& /*charset*/,
               const std::string& /*base_uri*/)
  : _implementation{nullptr} {

  // TODO
}

reader::reader(FILE* const stream)
  : _implementation{rdf_reader_for(stream)} {

  if (!_implementation) {
    throw std::invalid_argument{"unable to guess the input content type"};
  }
}

reader::reader(FILE* const stream,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation{rdf_reader_for(stream, content_type.c_str(), charset.c_str(), base_uri.c_str())} {

  if (!_implementation) {
    char error_message[1024];
    std::snprintf(error_message, sizeof(error_message),
      "unknown content type: %s", content_type.c_str());
    throw std::invalid_argument{error_message};
  }
}

reader::~reader() noexcept = default;

reader::reader(reader&&) /*noexcept*/ = default;

reader&
reader::operator=(reader&&) /*noexcept*/ = default;

void
reader::read_triples(std::function<void (std::unique_ptr<rdf::triple>)> callback) {
  assert(_implementation != nullptr);
  _implementation->read_triples(callback);
}

void
reader::read_quads(std::function<void (std::unique_ptr<rdf::quad>)> callback) {
  assert(_implementation != nullptr);
  _implementation->read_quads(callback);
}

void
reader::abort() {
  assert(_implementation != nullptr);
  _implementation->abort();
}
