/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_TRIX_H
#define RDFXX_WRITER_TRIX_H

#include "rdf++/writer/impl.h"

#include <libxml/xmlwriter.h>

class rdf::writer::trix : public rdf::writer::implementation {
public:
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

protected:
  void write_term(const term& term);

  void begin_document();

  void finish_document();

  void write_element(const char* name, const char* text);

  void begin_element(const char* name);

  void begin_element_with_ns(const char* name, const char* ns);

  void finish_element();

  void write_attribute(const char* name, const char* value);

  void write_text(const char* text);

private:
  xmlTextWriterPtr _writer = nullptr;
  rdf::uri_reference _context = {""};
};

#endif /* RDFXX_WRITER_TRIX_H */
