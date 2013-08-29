/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TRIPLE_H
#define RDFXX_TRIPLE_H

#include <memory> /* for std::unique_ptr */

namespace rdf {
  struct term;
  struct triple;
}

struct rdf::triple {
  std::unique_ptr<term> subject;
  std::unique_ptr<term> predicate;
  std::unique_ptr<term> object;

public:
  /**
   * Default constructor.
   */
  triple() noexcept;

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
  ~triple() noexcept;

  /**
   * Sets all terms to `nullptr`.
   */
  void reset() noexcept;
};

#endif /* RDFXX_TRIPLE_H */
