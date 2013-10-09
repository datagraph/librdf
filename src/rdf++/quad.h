/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_QUAD_H
#define RDFXX_QUAD_H

#include "term.h"

#include <memory> /* for std::unique_ptr */

namespace rdf {
  struct quad;
}

/**
 * An RDF quad.
 */
struct rdf::quad {
  std::unique_ptr<term> subject   {nullptr}; /** The subject term. */
  std::unique_ptr<term> predicate {nullptr}; /** The predicate term. */
  std::unique_ptr<term> object    {nullptr}; /** The object term. */
  std::unique_ptr<term> context   {nullptr}; /** The context/graph term. */

  /**
   * Default constructor.
   */
  quad() noexcept {}

  /**
   * Constructor.
   */
  quad(term* subject, term* predicate, term* object, term* context = nullptr) noexcept;

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

  /**
   * Destructor.
   */
  ~quad() noexcept = default;

  /**
   * Sets all terms to `nullptr`.
   */
  void reset() noexcept;
};

#endif /* RDFXX_QUAD_H */
