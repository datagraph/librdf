/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer/trix.h"

#include "xml_writer.h"

#include <cassert> /* for assert() */

namespace {
  struct implementation : public rdf::writer::implementation {
    implementation(FILE* stream,
      const char* content_type,
      const char* charset,
      const char* base_uri);
    virtual ~implementation() noexcept override;
    virtual void configure(const char* key, const char* value) override;
    virtual void begin() override;
    virtual void finish() override;
    virtual void write_triple(const rdf::triple& triple) override;
    virtual void write_quad(const rdf::quad& quad) override;
    virtual void write_comment(const char* comment) override;
    virtual void flush() override;

  protected:
    void write_term(const rdf::term& term);

  private:
    xml_writer _xml;
    rdf::uri_reference _context = {""};
  };
}

rdf::writer::implementation*
rdf_writer_for_trix(FILE* const stream,
                    const char* const content_type,
                    const char* const charset,
                    const char* const base_uri) {
  return new implementation(stream, content_type, charset, base_uri);
}

implementation::implementation(FILE* const stream,
                               const char* const content_type,
                               const char* const charset,
                               const char* const base_uri)
  : _xml(stream) {
  assert(stream != nullptr);
  (void)content_type, (void)charset, (void)base_uri;
}

implementation::~implementation() noexcept {}

void
implementation::configure(const char* const key,
                          const char* const value) {
  (void)key, (void)value; // TODO
}

void
implementation::begin() {
  _xml.set_indent(2);

  /* <?xml version="1.0" encoding="UTF-8"?> */
  _xml.begin_document();

  /* <trix xmlns="http://www.w3.org/2004/03/trix/trix-1/"> */
  _xml.begin_element_with_ns("trix", "http://www.w3.org/2004/03/trix/trix-1/");
}

void
implementation::finish() {
  if (_count) {
    _xml.finish_element(); /* </graph> */
  }

  _xml.finish_element(); /* </trix> */

  _xml.finish_document(); /* EOF */
}

void
implementation::write_triple(const rdf::triple& triple) {
  const bool context_changed = !_context.string.empty();

  if (context_changed || !_count) {
    if (_count) {
      _xml.finish_element(); /* </graph> */
    }
    _context.string.clear();
    _xml.begin_element("graph");
  }

  _xml.begin_element("triple");
  write_term(*triple.subject);
  write_term(*triple.predicate);
  write_term(*triple.object);
  _xml.finish_element();

  _count++;
}

void
implementation::write_quad(const rdf::quad& quad) {
  const bool context_changed = (!quad.context) ?
    !_context.string.empty() :
    (_context.string.compare((*quad.context).string) != 0);

  if (context_changed || !_count) {
    if (_count) {
      _xml.finish_element(); /* </graph> */
    }
    _xml.begin_element("graph");
    if (quad.context) {
      const auto& term = dynamic_cast<const rdf::uri_reference&>(*quad.context);
      _xml.write_element("uri", term.string.c_str());
      _context.string = term.string;
    }
    else {
      _context.string.clear();
    }
  }

  _xml.begin_element("triple");
  write_term(*quad.subject);
  write_term(*quad.predicate);
  write_term(*quad.object);
  _xml.finish_element();

  _count++;
}

void
implementation::write_comment(const char* const comment) {
  _xml.write_comment(comment);
}

void
implementation::flush() {
  _xml.flush();
}

void
implementation::write_term(const rdf::term& term_) {
  switch (term_.type) {
    case rdf::term_type::uri_reference: {
      const auto& term = dynamic_cast<const rdf::uri_reference&>(term_);
      _xml.write_element("uri", term.string.c_str());
      break;
    }
    case rdf::term_type::blank_node: {
      const auto& term = dynamic_cast<const rdf::blank_node&>(term_);
      _xml.write_element("id", term.string.c_str());
      break;
    }
    case rdf::term_type::plain_literal: {
      const auto& term = dynamic_cast<const rdf::plain_literal&>(term_);
      _xml.begin_element("plainLiteral");
      if (!term.language_tag.empty()) {
        _xml.write_attribute("xml:lang", term.language_tag.c_str());
      }
      _xml.write_text(term.string.c_str());
      _xml.finish_element();
      break;
    }
    case rdf::term_type::typed_literal: {
      const auto& term = dynamic_cast<const rdf::typed_literal&>(term_);
      _xml.begin_element("typedLiteral");
      _xml.write_attribute("datatype", term.datatype_uri.c_str());
      _xml.write_text(term.string.c_str());
      _xml.finish_element();
      break;
    }
    case rdf::term_type::none:
    default:
      assert(term_.type != rdf::term_type::none);
  }
}
