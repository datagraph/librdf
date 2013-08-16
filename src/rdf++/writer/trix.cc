/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer/trix.h"

#include <cassert> /* for assert() */

#include <libxml/xmlwriter.h>

namespace {
  struct implementation : public rdf::writer::implementation {
    implementation(FILE* stream,
      const char* content_type,
      const char* charset,
      const char* base_uri);
    virtual ~implementation() noexcept override;
    virtual void begin() override;
    virtual void finish() override;
    virtual void write_triple(const rdf::triple& triple) override;
    virtual void write_quad(const rdf::quad& quad) override;
    virtual void write_comment(const char* comment) override;
    virtual void flush() override;

  protected:
    void write_term(const rdf::term& term);

    void begin_document();

    void finish_document();

    void write_element(const char* name, const char* text);

    void begin_element(const char* name);

    void begin_element_with_ns(const char* name, const char* ns);

    void finish_element();

    void write_attribute(const char* name, const char* value);

    void write_text(const char* text);

  private:
    xmlTextWriterPtr _writer = nullptr;
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
                               const char* const base_uri) {
  assert(stream != nullptr);
  (void)content_type, (void)charset, (void)base_uri;

  xmlOutputBufferPtr output = xmlOutputBufferCreateFile(stream, nullptr);
  if (output == nullptr) {
    throw std::runtime_error("xmlOutputBufferCreateFile() failed"); // TODO
  }

  _writer = xmlNewTextWriter(output);
  if (_writer == nullptr) {
    (void)xmlOutputBufferClose(output);
    throw std::runtime_error("xmlNewTextWriter() failed"); // TODO
  }
}

implementation::~implementation() noexcept {
  if (_writer != nullptr) {
    xmlFreeTextWriter(_writer);
    _writer = nullptr;
  }
}

void
implementation::begin() {
  xmlTextWriterSetIndent(_writer, 1);
  xmlTextWriterSetIndentString(_writer, reinterpret_cast<const xmlChar*>("  ")); /* two spaces */

  /* <?xml version="1.0" encoding="UTF-8"?> */
  begin_document();

  /* <TriX xmlns="http://www.w3.org/2004/03/trix/trix-1/"> */
  begin_element_with_ns("TriX", "http://www.w3.org/2004/03/trix/trix-1/");
}

void
implementation::finish() {
  if (_count) {
    finish_element(); /* </graph> */
  }

  finish_element(); /* </TriX> */

  finish_document(); /* EOF */
}

void
implementation::write_triple(const rdf::triple& triple) {
  const bool context_changed = !_context.string.empty();

  if (context_changed || !_count) {
    if (_count) {
      finish_element(); /* </graph> */
    }
    _context.string.clear();
    begin_element("graph");
  }

  begin_element("triple");
  write_term(*triple.subject);
  write_term(*triple.predicate);
  write_term(*triple.object);
  finish_element();

  _count++;
}

void
implementation::write_quad(const rdf::quad& quad) {
  const bool context_changed = (!quad.context) ?
    _context.string.empty() :
    (_context.string.compare((*quad.context).string) == 0);

  if (context_changed || !_count) {
    if (_count) {
      finish_element(); /* </graph> */
    }
    begin_element("graph");
    if (quad.context) {
      const auto& term = dynamic_cast<const rdf::uri_reference&>(*quad.context);
      write_element("uri", term.string.c_str());
      _context.string = term.string;
    }
    else {
      _context.string.clear();
    }
  }

  begin_element("triple");
  write_term(*quad.subject);
  write_term(*quad.predicate);
  write_term(*quad.object);
  finish_element();

  _count++;
}

void
implementation::write_comment(const char* const comment) {
  (void)comment; // TODO
}

void
implementation::flush() {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterFlush */
  xmlTextWriterFlush(_writer);
}

void
implementation::write_term(const rdf::term& term_) {
  switch (term_.type) {
    case rdf::term_type::uri_reference: {
      const auto& term = dynamic_cast<const rdf::uri_reference&>(term_);
      write_element("uri", term.string.c_str());
      break;
    }
    case rdf::term_type::blank_node: {
      const auto& term = dynamic_cast<const rdf::blank_node&>(term_);
      write_element("id", term.string.c_str());
      break;
    }
    case rdf::term_type::plain_literal: {
      const auto& term = dynamic_cast<const rdf::plain_literal&>(term_);
      begin_element("plainLiteral");
      if (!term.language_tag.empty()) {
        write_attribute("xml:lang", term.language_tag.c_str());
      }
      write_text(term.string.c_str());
      finish_element();
      break;
    }
    case rdf::term_type::typed_literal: {
      const auto& term = dynamic_cast<const rdf::typed_literal&>(term_);
      begin_element("typedLiteral");
      write_attribute("datatype", term.datatype_uri.c_str());
      write_text(term.string.c_str());
      finish_element();
      break;
    }
    case rdf::term_type::none:
    default:
      assert(term.type != rdf::term_type::none);
  }
}

void
implementation::begin_document() {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterStartDocument */
  xmlTextWriterStartDocument(_writer, "1.0", "UTF-8", nullptr);
}

void
implementation::finish_document() {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterEndDocument */
  xmlTextWriterEndDocument(_writer);
}

void
implementation::write_element(const char* const name,
                              const char* const text) {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterWriteElement */
  xmlTextWriterWriteElement(_writer,
    reinterpret_cast<const xmlChar*>(name),
    reinterpret_cast<const xmlChar*>(text));
}

void
implementation::begin_element(const char* const name) {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterStartElement */
  xmlTextWriterStartElement(_writer,
    reinterpret_cast<const xmlChar*>(name));
}

void
implementation::begin_element_with_ns(const char* const name,
                                      const char* const ns) {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterStartElementNS */
  xmlTextWriterStartElementNS(_writer, nullptr,
    reinterpret_cast<const xmlChar*>(name),
    reinterpret_cast<const xmlChar*>(ns));
}

void
implementation::finish_element() {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterEndElement */
  //xmlTextWriterEndElement(_writer);
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterFullEndElement */
  xmlTextWriterFullEndElement(_writer);
}

void
implementation::write_attribute(const char* const name,
                                const char* const value) {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterWriteAttribute */
  xmlTextWriterWriteAttribute(_writer,
    reinterpret_cast<const xmlChar*>(name),
    reinterpret_cast<const xmlChar*>(value));
}

void
implementation::write_text(const char* const text) {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterWriteString */
  xmlTextWriterWriteString(_writer,
    reinterpret_cast<const xmlChar*>(text));
}
