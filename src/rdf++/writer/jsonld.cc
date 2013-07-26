/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer/jsonld.h"

using namespace rdf;

class writer::jsonld : public writer::implementation {
  public:
    jsonld();

    jsonld(
      const std::string& file_path,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    jsonld(
      std::ostream& stream,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    jsonld(
      FILE* stream,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    virtual ~jsonld() override;

    virtual void begin() override;

    virtual void finish() override;

    virtual void flush() override;

    virtual void write_triple(const triple& triple) override;

    virtual void write_quad(const quad& quad) override;
};

writer::jsonld::jsonld() {
  // TODO
}

writer::jsonld::~jsonld() {
  // TODO
}

void
writer::jsonld::begin() {
  // TODO
}

void
writer::jsonld::finish() {
  // TODO
}

void
writer::jsonld::flush() {
  // TODO
}

void
writer::jsonld::write_triple(const triple& triple) {
  (void)triple; // TODO
}

void
writer::jsonld::write_quad(const quad& quad) {
  (void)quad; // TODO
}
