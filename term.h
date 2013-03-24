/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TERM_H
#define RDFXX_TERM_H

#include <cassert> /* for assert() */
#include <string>  /* for std::string */

namespace rdf {
  enum class term_type : int {
    none = 0,
    uri_reference,
    blank_node,
    plain_literal,
    typed_literal,
  };

  enum class term_position : int {
    any       = -1,
    subject   = 0,
    predicate = 1,
    object    = 2,
    context   = 3,
  };

  /**
   * Abstract base class for RDF terms.
   */
  struct term {
    public:
      const term_type type;
      std::string string;

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
      term(const term_type type, const std::string& string)
        : type(type), string(string) {}

      term(const term_type type, const char* const string)
        : type(type), string(string) {}
  };

  /**
   * CRTP base class for RDF terms.
   */
  template <class Derived>
  class clonable_term : public term {
    public:
      virtual term* clone() const override {
        return new Derived(static_cast<Derived const&>(*this));
      }

    protected:
      clonable_term(const term_type type, const std::string& string)
        : term(type, string) {}

      clonable_term(const term_type type, const char* const string)
        : term(type, string) {}
  };

  /**
   * @see http://www.w3.org/TR/rdf-concepts/#section-Graph-URIref
   */
  struct uri_reference : public clonable_term<uri_reference> {
    public:
      uri_reference(const std::string& uri_string)
        : clonable_term<uri_reference>(term_type::uri_reference, uri_string) {}

      uri_reference(const char* const uri_string)
        : clonable_term<uri_reference>(term_type::uri_reference, uri_string) {}
  };

  /**
   * @see http://www.w3.org/TR/rdf-concepts/#section-blank-nodes
   */
  struct blank_node : public clonable_term<blank_node> {
    public:
      blank_node(const std::string& node_label)
        : clonable_term<blank_node>(term_type::blank_node, node_label) {}

      blank_node(const char* const node_label)
        : clonable_term<blank_node>(term_type::blank_node, node_label) {}
  };

  /**
   * @see http://www.w3.org/TR/rdf-concepts/#dfn-plain-literal
   */
  struct plain_literal : public clonable_term<plain_literal> {
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
   * @see http://www.w3.org/TR/rdf-concepts/#dfn-typed-literal
   */
  struct typed_literal : public clonable_term<typed_literal> {
    public:
      std::string datatype_uri;

      typed_literal(const std::string& lexical_form,
                    const std::string& datatype_uri)
        : clonable_term<typed_literal>(term_type::typed_literal, lexical_form),
          datatype_uri(datatype_uri) {
        assert(!datatype_uri.empty());
      }

      typed_literal(const char* const lexical_form,
                    const char* const datatype_uri)
        : clonable_term<typed_literal>(term_type::typed_literal, lexical_form),
          datatype_uri(datatype_uri) {
        assert(datatype_uri != nullptr);
      }
  };

  extern const term* const default_context;
}

#endif /* RDFXX_TERM_H */
