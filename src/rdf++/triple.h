/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TRIPLE_H
#define RDFXX_TRIPLE_H

#include <memory> /* for std::unique_ptr */

#include "rdf++/term.h"

namespace rdf {
  struct triple {
    public:
      std::unique_ptr<term> subject;
      std::unique_ptr<term> predicate;
      std::unique_ptr<term> object;

      triple(term* subject, term* predicate, term* object);

      triple(const term* subject, const term* predicate, const term* object);

      triple(const term& subject, const term& predicate, const term& object);
  };
}

#endif /* RDFXX_TRIPLE_H */
