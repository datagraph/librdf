/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TERM_H
#define RDFXX_TERM_H

#include <cassert>   /* for assert() */
#include <memory>    /* for std::unique_ptr */
#include <stdexcept> /* for std::invalid_argument */
#include <string>    /* for std::string */

namespace rdf {
  enum class term_type : int {
    none          = 0,
    uri_reference = 1,
    blank_node    = 2,
    plain_literal = 3,
    typed_literal = 4,
  };

  enum class term_position : int {
    any       = -1,
    subject   = 0,
    predicate = 1,
    object    = 2,
    context   = 3,
  };

  struct term;
  template <class Derived> class clonable_term;
  struct uri_reference;
  struct blank_node;
  struct plain_literal;
  struct typed_literal;

  extern const term* const default_context;
}

/**
 * Abstract base class for RDF terms.
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

  inline bool is_uri_reference() const {
    return type == term_type::uri_reference;
  }

  inline bool is_blank_node() const {
    return type == term_type::blank_node;
  }

  inline bool is_plain_literal() const {
    return type == term_type::plain_literal;
  }

  inline bool is_typed_literal() const {
    return type == term_type::typed_literal;
  }

protected:
  term(const term_type type)
    : type(type), string() {}

  term(const term_type type, const std::string& string)
    : type(type), string(string) {}

  term(const term_type type, const char* const string)
    : type(type), string(string ? string : "") {}
};

/**
 * CRTP base class for RDF terms.
 */
template <class Derived>
class rdf::clonable_term : public rdf::term {
public:
  virtual term* clone() const override {
    return new Derived(static_cast<Derived const&>(*this));
  }

protected:
  clonable_term(const term_type type)
    : term(type) {}

  clonable_term(const term_type type, const std::string& string)
    : term(type, string) {}

  clonable_term(const term_type type, const char* const string)
    : term(type, string) {}
};

/**
 * URI reference.
 *
 * @see http://www.w3.org/TR/rdf-concepts/#section-Graph-URIref
 */
struct rdf::uri_reference : public rdf::clonable_term<uri_reference> {
public:
  uri_reference(const std::string& uri_string)
    : clonable_term<uri_reference>(term_type::uri_reference, uri_string) {}

  uri_reference(const char* const uri_string)
    : clonable_term<uri_reference>(term_type::uri_reference, uri_string) {}
};

/**
 * Blank node.
 *
 * @see http://www.w3.org/TR/rdf-concepts/#section-blank-nodes
 */
struct rdf::blank_node : public rdf::clonable_term<blank_node> {
public:
  blank_node();

  blank_node(const std::string& node_label)
    : clonable_term<blank_node>(term_type::blank_node, node_label) {}

  blank_node(const char* const node_label)
    : clonable_term<blank_node>(term_type::blank_node, node_label) {}
};

/**
 * Plain literal.
 *
 * @see http://www.w3.org/TR/rdf-concepts/#dfn-plain-literal
 */
struct rdf::plain_literal : public clonable_term<plain_literal> {
public:
  std::string language_tag;

  plain_literal(const std::string& lexical_form)
    : clonable_term<plain_literal>(term_type::plain_literal, lexical_form),
      language_tag() {}

  plain_literal(const std::string& lexical_form,
                const std::string& language_tag)
    : clonable_term<plain_literal>(term_type::plain_literal, lexical_form),
      language_tag(language_tag) {
    if (!language_tag.empty()) {
      // TODO: normalize the language tag to lower case, if needed.
    }
  }

  plain_literal(const char* const lexical_form,
                const char* const language_tag = nullptr)
    : clonable_term<plain_literal>(term_type::plain_literal, lexical_form),
      language_tag(language_tag != nullptr ? language_tag : "") {
    if (language_tag != nullptr) {
      // TODO: normalize the language tag to lower case, if needed.
    }
  }
};

/**
 * Typed literal.
 *
 * @see http://www.w3.org/TR/rdf-concepts/#dfn-typed-literal
 */
struct rdf::typed_literal : public rdf::clonable_term<typed_literal> {
public:
  std::string datatype_uri;

  typed_literal(const std::string& lexical_form,
                const std::string& datatype_uri)
    : clonable_term<typed_literal>(term_type::typed_literal, lexical_form),
      datatype_uri(datatype_uri) {
    if (datatype_uri.empty()) {
      throw std::invalid_argument("datatype URI cannot be empty");
    }
  }

  typed_literal(const char* const lexical_form,
                const char* const datatype_uri)
    : clonable_term<typed_literal>(term_type::typed_literal, lexical_form),
      datatype_uri(datatype_uri) {
    if (datatype_uri == nullptr) {
      throw std::invalid_argument("datatype URI cannot be nullptr");
    }
  }
};

#endif /* RDFXX_TERM_H */
