/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer.h"
#include "rdf++/writer/impl.h"

#ifndef DISABLE_JSONLD
#include "rdf++/writer/jsonld.h"
#endif

#ifndef DISABLE_RAPTOR
#include "rdf++/writer/raptor.h"
#endif

#ifndef DISABLE_TRIX
#include "rdf++/writer/trix.h"
#endif

#ifndef DISABLE_XSLT
#include "rdf++/writer/xslt.h"
#endif

#include "rdf++/format.h"
#include "rdf++/quad.h"
#include "rdf++/raptor.h"
#include "rdf++/term.h"
#include "rdf++/triple.h"

#include <cassert>   /* for assert() */
#include <cstring>   /* for std::strcmp() */
#include <stdexcept> /* for std::invalid_argument, std::runtime_error */

using namespace rdf;

static rdf::writer::implementation*
rdf_writer_for(FILE* stream,
               const char* const content_type,
               const char* const charset,
               const char* const base_uri) {
  const format* const format = format::find_for_content_type(content_type);
  if (format == nullptr) {
    return nullptr; /* unknown content type */
  }

#ifndef DISABLE_JSONLD
  if (std::strcmp("jsonld", format->module_name) == 0) {
    return rdf_writer_for_jsonld(stream, content_type, charset, base_uri);
  }
#endif

#ifndef DISABLE_RAPTOR
  if (std::strcmp("raptor", format->module_name) == 0) {
    return rdf_writer_for_raptor(stream, content_type, charset, base_uri);
  }
#endif

#ifndef DISABLE_TRIX
  if (std::strcmp("trix", format->module_name) == 0) {
    return rdf_writer_for_trix(stream, content_type, charset, base_uri);
  }
#endif

#ifndef DISABLE_XSLT
  if (std::strcmp("xslt", format->module_name) == 0) {
    return rdf_writer_for_xslt(stream, content_type, charset, base_uri);
  }
#endif

  (void)stream, (void)charset, (void)base_uri;
  return nullptr; /* no implementation available */
}

writer::writer(const std::string& file_path,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(nullptr) {
  (void)file_path, (void)content_type, (void)charset, (void)base_uri; // TODO
}

writer::writer(std::ostream& stream,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(nullptr) {
  (void)stream, (void)content_type, (void)charset, (void)base_uri; // TODO
}

writer::writer(FILE* const stream,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(rdf_writer_for(stream, content_type.c_str(), charset.c_str(), base_uri.c_str())) {
  if (!_implementation) {
    throw std::invalid_argument("unknown content type: " + content_type);
  }
}

writer::~writer() = default;

void
writer::configure(const char* const key,
                  const char* const value) {
  assert(_implementation != nullptr);
  assert(key != nullptr); // FIXME
  _implementation->configure(key, value);
}

void
writer::define_prefix(const std::string& prefix,
                      const std::string& uri_string) {
  assert(_implementation != nullptr);
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

void
writer::write_comment(const char* const comment) {
  assert(_implementation != nullptr);
  _implementation->write_comment(comment);
}
