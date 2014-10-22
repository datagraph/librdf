/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_READER_IMPL_H
#define RDFXX_READER_IMPL_H

#include "rdf++/reader.h"

#include <cstddef>    /* for std::size_t */
#include <functional> /* for std::function */

struct rdf::reader::implementation {
protected:
  /**
   * Default constructor.
   */
  implementation() noexcept = default;

public:
  /**
   * Copy constructor.
   */
  implementation(const implementation& other) noexcept = delete;

  /**
   * Move constructor.
   */
  implementation(implementation&& other) noexcept = default;

  /**
   * Destructor.
   */
  virtual ~implementation() noexcept = default;

  /**
   * Copy assignment operator.
   */
  implementation& operator=(const implementation& other) noexcept = delete;

  /**
   * Move assignment operator.
   */
  implementation& operator=(implementation&& other) noexcept = default;

  virtual void read_triples(std::function<void (std::unique_ptr<rdf::triple>)> callback) = 0;

  virtual void read_quads(std::function<void (std::unique_ptr<rdf::quad>)> callback) = 0;

  virtual void abort() = 0;

protected:
  std::size_t _count = 0;
};

#endif /* RDFXX_READER_IMPL_H */
