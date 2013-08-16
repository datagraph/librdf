/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_IMPL_H
#define RDFXX_WRITER_IMPL_H

#include "rdf++/writer.h"

#include <boost/noncopyable.hpp>

#include <cstddef> /* for std::size_t */

struct rdf::writer::implementation : private boost::noncopyable {
protected:
  implementation() noexcept {}

public:
  virtual ~implementation() noexcept {}

  virtual void configure(const char* key, const char* value) {
    (void)key, (void)value;
  }

  virtual void begin() {}

  virtual void finish() {}

  virtual void write_triple(const rdf::triple& triple) = 0;

  virtual void write_quad(const rdf::quad& quad) = 0;

  virtual void write_comment(const char* comment) { (void)comment; }

  virtual void flush() {}

protected:
  std::size_t _count = 0;
};

#endif /* RDFXX_WRITER_IMPL_H */
