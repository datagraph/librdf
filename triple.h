/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TRIPLE_H
#define RDFXX_TRIPLE_H

#include "rdf++/term.h"

namespace rdf {
  struct triple {
    public:
      const term& subject;
      const term& predicate;
      const term& object;

      triple(const term& subject, const term& predicate, const term& object)
        : subject(subject),
          predicate(predicate),
          object(object) {}
  };
}

#endif /* RDFXX_TRIPLE_H */
