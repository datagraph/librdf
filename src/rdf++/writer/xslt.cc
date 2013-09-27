/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "raptor.h"
#include "xslt.h"
#include "xslt_xqilla.h"

#include <cassert>      /* for assert() */
#include <cerrno>       /* for errno */
#include <cstdio>       /* for std::fclose(), std::fwrite(), std::tmpfile() */
#include <cstring>      /* for std::strcmp() */
#include <memory>       /* for std::unique_ptr */
#include <stdexcept>    /* for std::invalid_argument */
#include <system_error> /* for std::system_error */

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

  private:
    FILE* _xslt_stream {nullptr};
    FILE* _xml_stream {nullptr};
    std::unique_ptr<rdf::writer::implementation> _xml_writer {nullptr};
    std::string _xslt_template;
  };
}

rdf::writer::implementation*
rdf_writer_for_xslt(FILE* const stream,
                    const char* const content_type,
                    const char* const charset,
                    const char* const base_uri) {
  return new implementation(stream, content_type, charset, base_uri);
}

implementation::implementation(FILE* const stream,
                               const char* const content_type,
                               const char* const charset,
                               const char* const base_uri)
  : _xslt_stream(stream) {
  (void)content_type;

  _xml_stream = std::tmpfile();
  if (!_xml_stream) {
    throw std::system_error(errno, std::system_category());
  }

  _xml_writer.reset(rdf_writer_for_raptor(_xml_stream, "application/rdf+xml", charset, base_uri));
  assert(_xml_writer);
}

implementation::~implementation() noexcept {
  if (_xml_stream) {
    if (std::fclose(_xml_stream) == EOF) {
      /* Ignore any errors from std::fclose(). */
    }
    _xml_stream = nullptr;
  }
}

void
implementation::configure(const char* const key,
                          const char* const value) {
  if (!key) {
    throw std::invalid_argument("key cannot be nullptr");
  }
  if (!*key) {
    throw std::invalid_argument("key cannot be empty");
  }

  if (std::strcmp(key, "xslt-template") == 0) {
    if (!value) {
      throw std::invalid_argument("xslt-template value cannot be nullptr");
    }
    if (!*value) {
      throw std::invalid_argument("xslt-template value cannot be empty");
    }
    _xslt_template.assign(value);
  }

#if 0
  throw std::invalid_argument("unknown key");
#endif
}

void
implementation::begin() {
  _xml_writer->begin();
}

void
implementation::finish() {
  _xml_writer->finish();

  fflush(_xml_stream);
  rewind(_xml_stream);

  xslt_processor xslt_proc;
  xslt_template xslt(xslt_proc, _xslt_template);
  xslt.execute(_xml_stream, _xslt_stream);
}

void
implementation::write_triple(const rdf::triple& triple) {
  _xml_writer->write_triple(triple);
}

void
implementation::write_quad(const rdf::quad& quad) {
  _xml_writer->write_quad(quad);
}

void
implementation::write_comment(const char* const comment) {
  _xml_writer->write_comment(comment);
}

void
implementation::flush() {
  /* nothing to do */
}
