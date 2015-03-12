/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TERM_H
#define RDFXX_TERM_H

#include <cassert>   /* for assert() */
#include <cstddef>   /* for std::size_t */
#include <memory>    /* for std::unique_ptr */
#include <stdexcept> /* for std::invalid_argument */
#include <string>    /* for std::string */

////////////////////////////////////////////////////////////////////////////////

namespace rdf {
  enum class term_type : int {
    none          = 0,
    uri_reference = 1,
    blank_node    = 2,
    plain_literal = 3,
    typed_literal = 4,
  };

  enum class term_position : int {
    any           = -1,
    subject       = 0,
    predicate     = 1,
    object        = 2,
    context       = 3,
  };

  struct term;
  template <class Derived> class clonable_term;
  struct uri_reference;
  struct blank_node;
  struct plain_literal;
  struct typed_literal;

  extern const term* const default_context;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Abstract base class for RDF terms.
 *
 * @abstract
 */
struct rdf::term {
public:
  const term_type type;
  std::string string;

  /**
   * @throws std::invalid_argument if `type` is invalid
   */
  static std::unique_ptr<term> create(
    term_type type,
    const char* string,
    const char* language_tag = nullptr,
    const char* datatype_uri = nullptr);

  virtual ~term() = default;

  virtual term* clone() const = 0;

  /**
   * Returns the N-Quads byte size of this term.
   */
  virtual std::size_t size() const = 0;

  bool is_uri_reference() const noexcept {
    return type == term_type::uri_reference;
  }

  bool is_blank_node() const noexcept {
    return type == term_type::blank_node;
  }

  bool is_literal() const noexcept {
    return is_plain_literal() || is_typed_literal();
  }

  bool is_plain_literal() const noexcept {
    return type == term_type::plain_literal;
  }

  bool is_typed_literal() const noexcept {
    return type == term_type::typed_literal;
  }

protected:
  /**
   * Constructor.
   */
  term(const term_type type)
    : type{type}, string{} {}

  /**
   * Constructor.
   */
  term(const term_type type, const std::string& string)
    : type{type}, string{string} {}

  /**
   * Constructor.
   */
  term(const term_type type, const char* const string)
    : type{type}, string{string ? string : ""} {}
};

////////////////////////////////////////////////////////////////////////////////

/**
 * CRTP base class for RDF terms.
 *
 * @abstract
 */
template <class Derived>
class rdf::clonable_term : public rdf::term {
public:
  virtual term* clone() const override {
    return new Derived(static_cast<Derived const&>(*this));
  }

protected:
  /**
   * Constructor.
   */
  clonable_term(const term_type type)
    : term{type} {}

  /**
   * Constructor.
   */
  clonable_term(const term_type type, const std::string& string)
    : term{type, string} {}

  /**
   * Constructor.
   */
  clonable_term(const term_type type, const char* const string)
    : term{type, string} {}
};

////////////////////////////////////////////////////////////////////////////////

/**
 * URI reference.
 *
 * @see http://www.w3.org/TR/rdf-concepts/#section-Graph-URIref
 */
struct rdf::uri_reference : public rdf::clonable_term<uri_reference> {
public:
  /**
   * Constructor.
   */
  uri_reference(const std::string& uri_string)
    : clonable_term<uri_reference>{term_type::uri_reference, uri_string} {}

  /**
   * Constructor.
   */
  uri_reference(const char* const uri_string)
    : clonable_term<uri_reference>{term_type::uri_reference, uri_string} {}

  /**
   * @copydoc rdf::term::size()
   */
  virtual std::size_t size() const override;
};

////////////////////////////////////////////////////////////////////////////////

/**
 * Blank node.
 *
 * @see http://www.w3.org/TR/rdf-concepts/#section-blank-nodes
 */
struct rdf::blank_node : public rdf::clonable_term<blank_node> {
public:
  /**
   * Constructor.
   */
  blank_node();

  /**
   * Constructor.
   */
  blank_node(const std::string& node_label)
    : clonable_term<blank_node>{term_type::blank_node, node_label} {}

  /**
   * Constructor.
   */
  blank_node(const char* const node_label)
    : clonable_term<blank_node>{term_type::blank_node, node_label} {}

  /**
   * @copydoc rdf::term::size()
   */
  virtual std::size_t size() const override;
};

////////////////////////////////////////////////////////////////////////////////

/**
 * Plain literal.
 *
 * @see http://www.w3.org/TR/rdf-concepts/#dfn-plain-literal
 */
struct rdf::plain_literal : public clonable_term<plain_literal> {
public:
  std::string language_tag;

  /**
   * Constructor.
   */
  plain_literal(const std::string& lexical_form)
    : clonable_term<plain_literal>{term_type::plain_literal, lexical_form},
      language_tag{} {}

  /**
   * Constructor.
   */
  plain_literal(const std::string& lexical_form,
                const std::string& language_tag)
    : clonable_term<plain_literal>{term_type::plain_literal, lexical_form},
      language_tag{language_tag} {
    if (!language_tag.empty()) {
      // TODO: normalize the language tag to lower case, if needed.
    }
  }

  /**
   * Constructor.
   */
  plain_literal(const char* const lexical_form,
                const char* const language_tag = nullptr)
    : clonable_term<plain_literal>{term_type::plain_literal, lexical_form},
      language_tag{language_tag != nullptr ? language_tag : ""} {
    if (language_tag != nullptr) {
      // TODO: normalize the language tag to lower case, if needed.
    }
  }

  /**
   * @copydoc rdf::term::size()
   */
  virtual std::size_t size() const override;
};

////////////////////////////////////////////////////////////////////////////////

/**
 * Typed literal.
 *
 * @see http://www.w3.org/TR/rdf-concepts/#dfn-typed-literal
 */
struct rdf::typed_literal : public rdf::clonable_term<typed_literal> {
public:
  std::string datatype_uri;

  /**
   * Constructor.
   */
  typed_literal(const std::string& lexical_form,
                const std::string& datatype_uri)
    : clonable_term<typed_literal>{term_type::typed_literal, lexical_form},
      datatype_uri{datatype_uri} {
    if (datatype_uri.empty()) {
      throw std::invalid_argument("datatype URI cannot be empty");
    }
  }

  /**
   * Constructor.
   */
  typed_literal(const char* const lexical_form,
                const char* const datatype_uri)
    : clonable_term<typed_literal>{term_type::typed_literal, lexical_form},
      datatype_uri{datatype_uri} {
    if (datatype_uri == nullptr) {
      throw std::invalid_argument("datatype URI cannot be nullptr");
    }
  }

  /**
   * @copydoc rdf::term::size()
   */
  virtual std::size_t size() const override;
};

////////////////////////////////////////////////////////////////////////////////

#endif /* RDFXX_TERM_H */
