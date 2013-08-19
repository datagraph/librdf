/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_READER_IMPL_H
#define RDFXX_READER_IMPL_H

#include "rdf++/reader.h"

#include <boost/noncopyable.hpp>

#include <cstddef>    /* for std::size_t */
#include <functional> /* for std::function */

struct rdf::reader::implementation : private boost::noncopyable {
protected:
  implementation() noexcept {}

public:
  virtual ~implementation() noexcept {}

  virtual void read_triples(std::function<void (rdf::triple*)> callback) = 0;

  virtual void read_quads(std::function<void (rdf::quad*)> callback) = 0;

  virtual void abort() = 0;

protected:
  std::size_t _count = 0;
};

#endif /* RDFXX_READER_IMPL_H */
