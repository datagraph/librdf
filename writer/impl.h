/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_IMPL_H
#define RDFXX_WRITER_IMPL_H

#include "rdf++/writer.h"

namespace rdf {
  class writer::implementation : private boost::noncopyable {
    public:
      implementation() {}

      virtual ~implementation() {}

      virtual void begin() = 0;

      virtual void finish() = 0;

      virtual void flush() = 0;

      virtual void write_triple(const triple& triple) = 0;

      virtual void write_quad(const quad& quad) = 0;
  };
}

#endif /* RDFXX_WRITER_IMPL_H */
