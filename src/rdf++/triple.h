/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TRIPLE_H
#define RDFXX_TRIPLE_H

#include "term.h"

#include <memory> /* for std::unique_ptr */

namespace rdf {
  struct triple;
}

/**
 * An RDF triple.
 */
struct rdf::triple {
  std::unique_ptr<term> subject   {nullptr}; /** The subject term. */
  std::unique_ptr<term> predicate {nullptr}; /** The predicate term. */
  std::unique_ptr<term> object    {nullptr}; /** The object term. */

  /**
   * Default constructor.
   */
  triple() noexcept {}

  /**
   * Constructor.
   */
  triple(term* subject, term* predicate, term* object) noexcept;

  /**
   * Constructor.
   */
  triple(const term* subject, const term* predicate, const term* object);

  /**
   * Constructor.
   */
  triple(const term& subject, const term& predicate, const term& object);

  /**
   * Destructor.
   */
  ~triple() noexcept = default;

  /**
   * Sets all terms to `nullptr`.
   */
  void reset() noexcept;
};

#endif /* RDFXX_TRIPLE_H */
