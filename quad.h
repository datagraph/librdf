/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_QUAD_H
#define RDFXX_QUAD_H

#include "rdf++/term.h"

namespace rdf {
  struct quad {
    public:
      const term& subject;
      const term& predicate;
      const term& object;
      const term& context;

      quad(const term& subject, const term& predicate, const term& object)
        : subject(subject),
          predicate(predicate),
          object(object),
          context(*rdf::default_context) {}

      quad(const term& subject, const term& predicate, const term& object, const term& context)
        : subject(subject),
          predicate(predicate),
          object(object),
          context(context) {}
  };
}

#endif /* RDFXX_QUAD_H */
