/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_QUAD_H
#define RDFXX_QUAD_H

#include <memory> /* for std::unique_ptr */

#include "rdf++/term.h"

namespace rdf {
  struct quad;
}

struct rdf::quad {
  std::unique_ptr<term> subject;
  std::unique_ptr<term> predicate;
  std::unique_ptr<term> object;
  std::unique_ptr<term> context;

public:
  /**
   * Constructor.
   */
  quad(term* subject, term* predicate, term* object, term* context = nullptr);

  /**
   * Constructor.
   */
  quad(const term* subject, const term* predicate, const term* object, const term* context = nullptr);

  /**
   * Constructor.
   */
  quad(const term& subject, const term& predicate, const term& object);

  /**
   * Constructor.
   */
  quad(const term& subject, const term& predicate, const term& object, const term& context);
};

#endif /* RDFXX_QUAD_H */
