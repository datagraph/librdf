/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer.h"
#include "rdf++/writer/impl.h"
#include "rdf++/writer/jsonld.h"
#include "rdf++/writer/raptor.h"
#include "rdf++/writer/trix.h"

#include "rdf++/format.h"
#include "rdf++/quad.h"
#include "rdf++/raptor.h"
#include "rdf++/term.h"
#include "rdf++/triple.h"

#include <cassert>   /* for assert() */
#include <stdexcept> /* for std::invalid_argument, std::runtime_error */

using namespace rdf;

writer::writer(const std::string& file_path,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(nullptr/*new writer::implementation(
      file_path, content_type, charset, base_uri)*/) {}

writer::writer(std::ostream& stream,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(nullptr/*new writer::implementation(
      stream, content_type, charset, base_uri)*/) {}

writer::writer(FILE* const stream,
               const std::string& content_type,
               const std::string& charset,
               const std::string& base_uri)
  : _implementation(nullptr/*new writer::implementation(
      stream, content_type, charset, base_uri)*/) {}

writer::~writer() = default;

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
