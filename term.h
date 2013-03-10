/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TERM_H
#define RDFXX_TERM_H

#include <string> /* for std::string */

namespace rdf {
  enum term_type : int {
    none      = 0,
    uri       = 1,
    node      = 2,
    literal   = 3,
  };

  enum term_position : int {
    any       = -1,
    subject   = 0,
    predicate = 1,
    object    = 2,
    context   = 3,
  };

  struct term {
    public:
      term_type type;
      std::string value;
      std::string language;
      std::string datatype;

      term(term_type type, const std::string& value);

      ~term();
  };
}

#endif /* RDFXX_TERM_H */
