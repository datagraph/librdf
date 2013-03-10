/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TRIPLE_H
#define RDFXX_TRIPLE_H

namespace rdf {
  struct term;

  struct triple {
    public:
      term* subject   = nullptr;
      term* predicate = nullptr;
      term* object    = nullptr;

      triple(term* subject, term* predicate, term* object)
        : subject(subject),
          predicate(predicate),
          object(object) {}
  };
}

#endif /* RDFXX_TRIPLE_H */
