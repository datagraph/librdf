/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/reader.h"
#include "rdf++/reader/impl.h"

#ifndef DISABLE_JSONLD
#include "rdf++/reader/jsonld.h"
#endif

#ifndef DISABLE_RAPTOR
#include "rdf++/reader/raptor.h"
#endif

#ifndef DISABLE_TRIX
#include "rdf++/reader/trix.h"
#endif

#include "rdf++/format.h"
#include "rdf++/quad.h"
#include "rdf++/raptor.h"
#include "rdf++/term.h"
#include "rdf++/triple.h"

#include <cassert>    /* for assert() */
#include <cstring>    /* for std::strcmp() */
#include <functional> /* for std::function */
#include <stdexcept>  /* for std::invalid_argument, std::runtime_error */

using namespace rdf;

static rdf::reader::implementation*
rdf_reader_for(FILE* stream,
               const char* const content_type = nullptr,
               const char* const charset = nullptr,
               const char* const base_uri = nullptr) {
#ifndef DISABLE_RAPTOR
  /* Only the Raptor implementation supports content autodetection at the moment: */
  if (!content_type) {
    return rdf_reader_for_raptor(stream, content_type, charset, base_uri);
  }
#endif

  const format* const format = format::find_for_content_type(content_type);
  if (format == nullptr) {
    return nullptr; /* unknown content type */
  }

#ifndef DISABLE_JSONLD
  if (std::strcmp("jsonld", format->module_name) == 0) {
    return rdf_reader_for_jsonld(stream, content_type, charset, base_uri);
  }
#endif

#ifndef DISABLE_RAPTOR
  if (std::strcmp("raptor", format->module_name) == 0) {
    return rdf_reader_for_raptor(stream, content_type, charset, base_uri);
  }
#endif

#ifndef DISABLE_TRIX
  if (std::strcmp("trix", format->module_name) == 0) {
    return rdf_reader_for_trix(stream, content_type, charset, base_uri);
  }
#endif

  (void)stream, (void)charset, (void)base_uri;
  return nullptr; /* no implementation available */
}

reader::reader(const std::string& file_path,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(nullptr) {
  (void)file_path, (void)content_type, (void)charset, (void)base_uri; // TODO
}

reader::reader(std::istream& stream,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(nullptr) {
  (void)stream, (void)content_type, (void)charset, (void)base_uri; // TODO
}

reader::reader(FILE* const stream)
  : _implementation(rdf_reader_for(stream)) {
  if (!_implementation) {
    throw std::invalid_argument("unable to guess the input content type");
  }
}

reader::reader(FILE* const stream,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(rdf_reader_for(stream, content_type.c_str(), charset.c_str(), base_uri.c_str())) {
  if (!_implementation) {
    throw std::invalid_argument("unknown content type: " + content_type);
  }
}

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
