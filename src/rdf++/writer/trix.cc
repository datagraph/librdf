/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer/trix.h"

#include <cassert> /* for assert() */

using namespace rdf;

writer::trix::trix(const std::string& file_path,
                   const std::string& content_type,
                   const std::string& charset,
                   const std::string& base_uri) {
  assert(!file_path.empty());
  (void)file_path, (void)content_type, (void)charset, (void)base_uri;
  throw std::runtime_error("not implemented"); // TODO
}

writer::trix::trix(std::ostream& stream,
                   const std::string& content_type,
                   const std::string& charset,
                   const std::string& base_uri) {
  (void)stream, (void)content_type, (void)charset, (void)base_uri;
  throw std::runtime_error("not implemented"); // TODO
}

writer::trix::trix(FILE* const stream,
                   const std::string& content_type,
                   const std::string& charset,
                   const std::string& base_uri) {
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

writer::trix::~trix() {
  if (_writer != nullptr) {
    xmlFreeTextWriter(_writer);
    _writer = nullptr;
  }
}

void
writer::trix::begin() {
  xmlTextWriterSetIndent(_writer, 1);
  xmlTextWriterSetIndentString(_writer, reinterpret_cast<const xmlChar*>("  ")); /* two spaces */

  /* <?xml version="1.0" encoding="UTF-8"?> */
  begin_document();

  /* <TriX xmlns="http://www.w3.org/2004/03/trix/trix-1/"> */
  begin_element_with_ns("TriX", "http://www.w3.org/2004/03/trix/trix-1/");
}

void
writer::trix::finish() {
  if (_count) {
    finish_element(); /* </graph> */
  }

  finish_element(); /* </TriX> */

  finish_document(); /* EOF */
}

void
writer::trix::flush() {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterFlush */
  xmlTextWriterFlush(_writer);
}

void
writer::trix::write_triple(const triple& triple) {
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
writer::trix::write_quad(const quad& quad) {
  const bool context_changed = (!quad.context) ?
    _context.string.empty() :
    (_context.string.compare((*quad.context).string) == 0);

  if (context_changed || !_count) {
    if (_count) {
      finish_element(); /* </graph> */
    }
    begin_element("graph");
    if (quad.context) {
      const auto& term = dynamic_cast<const uri_reference&>(*quad.context);
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
writer::trix::write_term(const term& term_) {
  switch (term_.type) {
    case term_type::uri_reference: {
      const auto& term = dynamic_cast<const uri_reference&>(term_);
      write_element("uri", term.string.c_str());
      break;
    }
    case term_type::blank_node: {
      const auto& term = dynamic_cast<const blank_node&>(term_);
      write_element("id", term.string.c_str());
      break;
    }
    case term_type::plain_literal: {
      const auto& term = dynamic_cast<const plain_literal&>(term_);
      begin_element("plainLiteral");
      if (!term.language_tag.empty()) {
        write_attribute("xml:lang", term.language_tag.c_str());
      }
      write_text(term.string.c_str());
      finish_element();
      break;
    }
    case term_type::typed_literal: {
      const auto& term = dynamic_cast<const typed_literal&>(term_);
      begin_element("typedLiteral");
      write_attribute("datatype", term.datatype_uri.c_str());
      write_text(term.string.c_str());
      finish_element();
      break;
    }
    case term_type::none:
    default:
      assert(term.type != term_type::none);
  }
}

void
writer::trix::begin_document() {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterStartDocument */
  xmlTextWriterStartDocument(_writer, "1.0", "UTF-8", nullptr);
}

void
writer::trix::finish_document() {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterEndDocument */
  xmlTextWriterEndDocument(_writer);
}

void
writer::trix::write_element(const char* const name,
                            const char* const text) {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterWriteElement */
  xmlTextWriterWriteElement(_writer,
    reinterpret_cast<const xmlChar*>(name),
    reinterpret_cast<const xmlChar*>(text));
}

void
writer::trix::begin_element(const char* const name) {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterStartElement */
  xmlTextWriterStartElement(_writer,
    reinterpret_cast<const xmlChar*>(name));
}

void
writer::trix::begin_element_with_ns(const char* const name,
                                    const char* const ns) {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterStartElementNS */
  xmlTextWriterStartElementNS(_writer, nullptr,
    reinterpret_cast<const xmlChar*>(name),
    reinterpret_cast<const xmlChar*>(ns));
}

void
writer::trix::finish_element() {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterEndElement */
  //xmlTextWriterEndElement(_writer);
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterFullEndElement */
  xmlTextWriterFullEndElement(_writer);
}

void
writer::trix::write_attribute(const char* const name,
                              const char* const value) {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterWriteAttribute */
  xmlTextWriterWriteAttribute(_writer,
    reinterpret_cast<const xmlChar*>(name),
    reinterpret_cast<const xmlChar*>(value));
}

void
writer::trix::write_text(const char* const text) {
  /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterWriteString */
  xmlTextWriterWriteString(_writer,
    reinterpret_cast<const xmlChar*>(text));
}
