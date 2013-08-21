/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/reader/trix.h"

#include "rdf++/quad.h"
#include "rdf++/term.h"
#include "rdf++/triple.h"

#include "xml_reader.h"

#include <memory>  /* for std::unique_ptr */
#include <cstring> /* for std::strcmp() */

namespace {
  enum class trix_element {
    unknown = 0,
    TriX,
    graph,
    triple,
    id,
    uri,
    plain_literal,
    typed_literal,
  };

  enum class trix_state {start, document, graph, triple, eof, abort};

  struct trix_context {
    trix_state state = trix_state::start;
    trix_element element = trix_element::unknown;
    unsigned int term_pos = 0;
    std::unique_ptr<rdf::term> terms[3] = {nullptr, nullptr, nullptr};
    std::unique_ptr<rdf::uri_reference> graph = nullptr;
    std::function<void (rdf::triple*)> triple_callback = nullptr;
    std::function<void (rdf::quad*)> quad_callback = nullptr;
  };

  struct implementation : public rdf::reader::implementation {
    implementation(FILE* stream,
      const char* content_type,
      const char* charset,
      const char* base_uri);
    virtual ~implementation() noexcept override;
    virtual void read_triples(std::function<void (rdf::triple*)> callback) override;
    virtual void read_quads(std::function<void (rdf::quad*)> callback) override;
    virtual void abort() override;
  protected:
    void read_with_context(trix_context& context);
    void ensure_state(trix_context& context, trix_state state);
    void assert_state(trix_context& context, trix_state state);
    void enter_state(trix_context& context, trix_state state);
    void leave_state(trix_context& context, trix_state state);
    void change_state(trix_context& context, trix_state state);
    void ensure_depth(trix_context& context, unsigned int depth);
    void record_term(trix_context& context);
    void begin_element(trix_context& context);
    void finish_element(trix_context& context);
    rdf::term* construct_term(trix_context& context);
    void throw_error(trix_context& context, const char* what);
  private:
    xml_reader _reader;
  };
}

rdf::reader::implementation*
rdf_reader_for_trix(FILE* const stream,
                    const char* const content_type,
                    const char* const charset,
                    const char* const base_uri) {
  return new implementation(stream, content_type, charset, base_uri);
}

implementation::implementation(FILE* const stream,
                               const char* const content_type,
                               const char* const charset,
                               const char* const base_uri)
  : _reader(stream, base_uri, charset) {
  assert(stream != nullptr);
  (void)content_type;
}

implementation::~implementation() noexcept = default;

static trix_element
intern_trix_element(const char* const element_name) {
  switch (*element_name) {
    case 'T':
      if (std::strcmp(element_name, "TriX") == 0) {
        return trix_element::TriX;
      }
      break;
    case 'g':
      if (std::strcmp(element_name, "graph") == 0) {
        return trix_element::graph;
      }
      break;
    case 't':
      if (std::strcmp(element_name, "triple") == 0) {
        return trix_element::triple;
      }
      if (std::strcmp(element_name, "typedLiteral") == 0) {
        return trix_element::typed_literal;
      }
      break;
    case 'i':
      if (std::strcmp(element_name, "id") == 0) {
        return trix_element::id;
      }
      break;
    case 'u':
      if (std::strcmp(element_name, "uri") == 0) {
        return trix_element::uri;
      }
      break;
    case 'p':
      if (std::strcmp(element_name, "plainLiteral") == 0) {
        return trix_element::plain_literal;
      }
      break;
    default:
      break;
  }
  return trix_element::unknown;
}

void
implementation::read_triples(std::function<void (rdf::triple*)> callback) {
  trix_context context;
  context.triple_callback = callback;
  read_with_context(context);
}

void
implementation::read_quads(std::function<void (rdf::quad*)> callback) {
  trix_context context;
  context.quad_callback = callback;
  read_with_context(context);
}

void
implementation::abort() {
  // TODO
}

void
implementation::read_with_context(trix_context& context) {
  while (_reader.read()) {
    switch (_reader.node_type()) {
      case xml_node_type::element:
        begin_element(context);
        break;

      case xml_node_type::end_element:
        finish_element(context);
        break;

      default: /* ignored */
        break;
    }
  }
}

void
implementation::ensure_state(trix_context& context, const trix_state state) {
  if (context.state != state) {
    throw_error(context, "mismatched nesting of elements in TriX input");
  }
}

void
implementation::assert_state(trix_context& context, const trix_state state) {
  (void)state, (void)context;
  assert(context.state == state);
}

void
implementation::enter_state(trix_context& context, const trix_state state) {
  switch (state) {
    case trix_state::graph:
      context.graph.reset();
      break;

    case trix_state::triple:
      context.term_pos = 0;
      break;

    default:
      break;
  }
}

void
implementation::leave_state(trix_context& context, const trix_state state) {
  switch (state) {
    case trix_state::triple:
      if (context.quad_callback) {
        auto quad = new rdf::quad(
          context.terms[0].get(),
          context.terms[1].get(),
          context.terms[2].get(),
          context.graph ? (context.graph)->clone() : nullptr);
        context.quad_callback(quad);
      }
      else if (context.triple_callback) {
        auto triple = new rdf::triple(
          context.terms[0].get(),
          context.terms[1].get(),
          context.terms[2].get());
        context.triple_callback(triple);
      }
      for (auto i = 0U; i < 3; i++) {
        if (context.quad_callback || context.triple_callback) {
          /* The callback took ownership of the terms: */
          context.terms[i].release();
        }
        else {
          context.terms[i].reset();
        }
      }
      context.term_pos = 0;
      break;

    default:
      break;
  }
}

void
implementation::change_state(trix_context& context, const trix_state state) {
  leave_state(context, context.state);
  context.state = state;
  enter_state(context, context.state);
}

void
implementation::ensure_depth(trix_context& context, const unsigned int depth) {
  if (_reader.depth() != depth) {
    throw_error(context, "incorrect nesting of elements in TriX input");
  }
}

void
implementation::record_term(trix_context& context) {
  if (context.term_pos >= 3) {
    throw_error(context, "too many elements inside <triple> element");
  }

  context.terms[context.term_pos].reset(construct_term(context));
  context.term_pos++;
}

void
implementation::begin_element(trix_context& context) {
  context.element = intern_trix_element(_reader.name());

  switch (context.element) {
    case trix_element::TriX:
      ensure_state(context, trix_state::start);
      ensure_depth(context, 0);
      change_state(context, trix_state::document);
      break;

    case trix_element::graph:
      ensure_state(context, trix_state::document);
      ensure_depth(context, 1);
      change_state(context, trix_state::graph);
      break;

    case trix_element::triple:
      ensure_state(context, trix_state::graph);
      ensure_depth(context, 2);
      change_state(context, trix_state::triple);
      break;

    case trix_element::uri:
      if (context.state == trix_state::graph) {
        ensure_depth(context, 2);
        if (context.graph) {
          throw_error(context, "repeated <uri> element inside <graph> element");
        }
        context.graph.reset(new rdf::uri_reference(_reader.read_string()));
        break;
      }
      ensure_state(context, trix_state::triple);
      ensure_depth(context, 3);
      record_term(context);
      break;

    case trix_element::id:
    case trix_element::plain_literal:
    case trix_element::typed_literal:
      ensure_state(context, trix_state::triple);
      ensure_depth(context, 3);
      record_term(context);
      break;

    default:
      throw_error(context, "unknown XML element in TriX input");
  }
}

void
implementation::finish_element(trix_context& context) {
  context.element = intern_trix_element(_reader.name());

  switch (context.element) {
    case trix_element::TriX:
      assert_state(context, trix_state::document);
      change_state(context, trix_state::eof);
      break;

    case trix_element::graph:
      assert_state(context, trix_state::graph);
      change_state(context, trix_state::document);
      break;

    case trix_element::triple:
      assert_state(context, trix_state::triple);
      change_state(context, trix_state::graph);
      break;

    case trix_element::uri:
      //assert_state(trix_state::triple, context); // FIXME: triple | graph
      break;

    case trix_element::id:
    case trix_element::plain_literal:
    case trix_element::typed_literal:
      assert_state(context, trix_state::triple);
      break;

    default: abort(); /* never reached */
  }
}

rdf::term*
implementation::construct_term(trix_context& context) {
  rdf::term* term = nullptr;

  const auto text = _reader.read_string();

  switch (context.element) {
    case trix_element::id: {
      term = new rdf::blank_node(text);
      break;
    }

    case trix_element::uri: {
      term = new rdf::uri_reference(text);
      break;
    }

    case trix_element::plain_literal: {
      term = new rdf::plain_literal(text, _reader.xml_lang());
      break;
    }

    case trix_element::typed_literal: {
      const auto datatype_uri = _reader.get_attribute("datatype");
      if (!datatype_uri) {
        throw_error(context, "missing 'datatype' attribute for <typedLiteral> element");
      }
      term = new rdf::typed_literal(text, datatype_uri);
      break;
    }

    default: abort(); /* never reached */
  }

  return term;
}

void
implementation::throw_error(trix_context& context,
                            const char* const what) {
  (void)context;
  throw rdf::reader_error(what, _reader.line_number(), _reader.column_number());
}
