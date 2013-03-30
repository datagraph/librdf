/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer/trix.h"

using namespace rdf;

class writer::trix : public writer::implementation {
  public:
    trix();

    trix(
      const std::string& file_path,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    trix(
      std::ostream& stream,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    trix(
      FILE* stream,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    virtual ~trix() override;

    virtual void begin() override;

    virtual void finish() override;

    virtual void flush() override;

    virtual void write_triple(const triple& triple) override;

    virtual void write_quad(const quad& quad) override;
};

writer::trix::trix() {
  // TODO
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
