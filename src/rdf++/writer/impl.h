/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_IMPL_H
#define RDFXX_WRITER_IMPL_H

#include "rdf++/writer.h"

#include <cstddef> /* for std::size_t */

struct rdf::writer::implementation {
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

  virtual void configure(const char* key, const char* value) {
    static_cast<void>(key);
    static_cast<void>(value);
  }

  virtual void define_prefix(const char* prefix, const char* uri_string) {
    static_cast<void>(prefix);
    static_cast<void>(uri_string);
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
