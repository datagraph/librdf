/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer/trix.h"

#include <cassert> /* for assert() */

using namespace rdf;

writer::trix::trix(const std::string& file_path,
                   const std::string& content_type,
                   const std::string& charset,
                   const std::string& base_uri) {
  assert(!file_path.empty());
  (void)file_path, (void)content_type, (void)charset, (void)base_uri;
}

writer::trix::trix(std::ostream& stream,
                   const std::string& content_type,
                   const std::string& charset,
                   const std::string& base_uri) {
  (void)stream, (void)content_type, (void)charset, (void)base_uri;
}

writer::trix::trix(FILE* const stream,
                   const std::string& content_type,
                   const std::string& charset,
                   const std::string& base_uri) {
  assert(stream != nullptr);
  (void)stream, (void)content_type, (void)charset, (void)base_uri;
}

writer::trix::~trix() {
  // TODO
}

void
writer::trix::begin() {
  // TODO
}

void
writer::trix::finish() {
  // TODO
}

void
writer::trix::flush() {
  // TODO
}

void
writer::trix::write_triple(const triple& triple) {
  (void)triple; // TODO
}

void
writer::trix::write_quad(const quad& quad) {
  (void)quad; // TODO
}
