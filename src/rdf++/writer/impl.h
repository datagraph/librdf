/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_IMPL_H
#define RDFXX_WRITER_IMPL_H

#include "rdf++/writer.h"

#include <cstddef> /* for std::size_t */

class rdf::writer::implementation : private boost::noncopyable {
  public:
    static writer::implementation* create(
      const std::string& file_path,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    static writer::implementation* create(
      std::ostream& stream,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    static writer::implementation* create(
      FILE* stream,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    virtual ~implementation() {}

    virtual void begin() = 0;

    virtual void finish() = 0;

    virtual void flush() = 0;

    virtual void write_triple(const rdf::triple& triple) = 0;

    virtual void write_quad(const rdf::quad& quad) = 0;

  protected:
    implementation() {}

    std::size_t _count = 0;
};

#endif /* RDFXX_WRITER_IMPL_H */
