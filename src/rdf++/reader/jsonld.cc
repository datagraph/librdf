/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/reader/jsonld.h"

#include <cassert> /* for assert() */

namespace {
  struct implementation : public rdf::reader::implementation {
    implementation(FILE* stream,
      const char* content_type,
      const char* charset,
      const char* base_uri);
    virtual ~implementation() noexcept override;
    virtual void read_triples(std::function<void (std::unique_ptr<rdf::triple>)> callback) override;
    virtual void read_quads(std::function<void (std::unique_ptr<rdf::quad>)> callback) override;
    virtual void abort() override;
  };
}

rdf::reader::implementation*
rdf_reader_for_jsonld(FILE* const stream,
                      const char* const content_type,
                      const char* const charset,
                      const char* const base_uri) {
  (void)stream, (void)content_type, (void)charset, (void)base_uri;
  return nullptr; // TODO
}
