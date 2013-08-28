/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TRIPLE_H
#define RDFXX_TRIPLE_H

#include <memory> /* for std::unique_ptr */

#include "rdf++/term.h"

namespace rdf {
  struct triple;
}

struct rdf::triple {
  std::unique_ptr<term> subject;
  std::unique_ptr<term> predicate;
  std::unique_ptr<term> object;

public:
  /**
   * Constructor.
   */
  triple(term* subject, term* predicate, term* object);

  /**
   * Constructor.
   */
  triple(const term* subject, const term* predicate, const term* object);

  /**
   * Constructor.
   */
  triple(const term& subject, const term& predicate, const term& object);
};

#endif /* RDFXX_TRIPLE_H */
