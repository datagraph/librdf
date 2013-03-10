/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_QUAD_H
#define RDFXX_QUAD_H

namespace rdf {
  struct term;

  struct quad {
    public:
      term* subject   = nullptr;
      term* predicate = nullptr;
      term* object    = nullptr;
      term* context   = nullptr;

      quad(term* subject, term* predicate, term* object)
        : subject(subject),
          predicate(predicate),
          object(object) {}

      quad(term* subject, term* predicate, term* object, term* context)
        : subject(subject),
          predicate(predicate),
          object(object),
          context(context) {}
  };
}

#endif /* RDFXX_QUAD_H */
