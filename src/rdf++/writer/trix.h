/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_TRIX_H
#define RDFXX_WRITER_TRIX_H

#include "rdf++/writer/impl.h"

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
};

#endif /* RDFXX_WRITER_TRIX_H */
