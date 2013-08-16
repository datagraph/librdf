/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer/xslt.h"

#include <cassert> /* for assert() */

namespace {
  struct implementation : public rdf::writer::implementation {
    implementation(FILE* stream,
      const char* content_type,
      const char* charset,
      const char* base_uri);
    virtual ~implementation() noexcept override;
    virtual void begin() override;
    virtual void finish() override;
    virtual void write_triple(const rdf::triple& triple) override;
    virtual void write_quad(const rdf::quad& quad) override;
    virtual void write_comment(const char* comment) override;
    virtual void flush() override;
  };
}

rdf::writer::implementation*
rdf_writer_for_xslt(FILE* const stream,
                    const char* const content_type,
                    const char* const charset,
                    const char* const base_uri) {
  (void)stream, (void)content_type, (void)charset, (void)base_uri;
  return nullptr; // TODO
}
