/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nquads.h"

#include <cassert> /* for assert() */

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
  };
}

rdf::writer::implementation*
rdf_writer_for_nquads(FILE* const stream,
                      const char* const content_type,
                      const char* const charset,
                      const char* const base_uri) {
  return new implementation(stream, content_type, charset, base_uri);
}

implementation::implementation(FILE* const stream,
                               const char* const content_type,
                               const char* const charset,
                               const char* const base_uri) {
  assert(stream != nullptr);
  (void)stream, (void)content_type, (void)charset, (void)base_uri;
}

implementation::~implementation() noexcept {}

void
implementation::configure(const char* const key,
                          const char* const value) {
  (void)key, (void)value; // TODO
}

void
implementation::begin() {
  // TODO
}

void
implementation::finish() {
  // TODO
}

void
implementation::write_triple(const rdf::triple& triple) {
  (void)triple; // TODO
}

void
implementation::write_quad(const rdf::quad& quad) {
  (void)quad; // TODO
}

void
implementation::write_comment(const char* const comment) {
  (void)comment; // TODO
}

void
implementation::flush() {
  // TODO
}
