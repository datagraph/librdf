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
  std::unique_ptr<term> subject;   /** The subject term. */
  std::unique_ptr<term> predicate; /** The predicate term. */
  std::unique_ptr<term> object;    /** The object term. */
  std::unique_ptr<term> context;   /** The context/graph term. */

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

  inline bool has_context() const noexcept {
    return context.get() != nullptr;
  }

  inline bool has_subject() const noexcept {
    return subject.get() != nullptr;
  }

  inline bool has_predicate() const noexcept {
    return predicate.get() != nullptr;
  }

  inline bool has_object() const noexcept {
    return object.get() != nullptr;
  }

  inline const std::unique_ptr<term>& operator[](const std::size_t index) const {
    switch (index) {
      case 0:  return context;
      case 1:  return subject;
      case 2:  return predicate;
      case 3:  return object;
      default: throw std::out_of_range("subscript must be 0..3");
    }
  }

  inline std::unique_ptr<term>& operator[](const std::size_t index) {
    switch (index) {
      case 0:  return context;
      case 1:  return subject;
      case 2:  return predicate;
      case 3:  return object;
      default: throw std::out_of_range("subscript must be 0..3");
    }
  }
};

#endif /* RDFXX_QUAD_H */
