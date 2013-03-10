/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_TERM_H
#define RDFXX_TERM_H

#include <string> /* for std::string */

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
   * Base class for RDF terms.
   */
  struct term {
    public:
      const term_type type;
      std::string string;

      term()
        : type(term_type::none), string() {}

      virtual ~term() = default;

    protected:
      term(const term_type type, const std::string& string)
        : type(type), string(string) {}
  };

  /**
   * @see http://www.w3.org/TR/rdf-concepts/#section-blank-nodes
   */
  struct blank_node : public term {
    public:
      blank_node(const std::string& node_label)
        : term(term_type::blank_node, node_label) {}
  };

  /**
   * @see http://www.w3.org/TR/rdf-concepts/#section-Graph-URIref
   */
  struct uri_reference : public term {
    public:
      uri_reference(const std::string& uri_string)
        : term(term_type::uri_reference, uri_string) {}
  };

  /**
   * @see http://www.w3.org/TR/rdf-concepts/#dfn-plain-literal
   */
  struct plain_literal : public term {
    public:
      std::string language_tag;

      plain_literal(const std::string& lexical_form)
        : term(term_type::plain_literal, lexical_form),
          language_tag() {}

      plain_literal(const std::string& lexical_form,
                    const std::string& language_tag)
        : term(term_type::plain_literal, lexical_form),
          language_tag(language_tag) {
        // TODO: normalize the language tag to lower case, if needed.
      }
  };

  /**
   * @see http://www.w3.org/TR/rdf-concepts/#dfn-typed-literal
   */
  struct typed_literal : public term {
    public:
      std::string datatype_uri;

      typed_literal(const std::string& lexical_form,
                    const std::string& datatype_uri)
        : term(term_type::typed_literal, lexical_form),
          datatype_uri(datatype_uri) {}
  };
}

#endif /* RDFXX_TERM_H */
