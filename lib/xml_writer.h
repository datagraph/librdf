/* This is free and unencumbered software released into the public domain. */

#ifndef XML_WRITER_H
#define XML_WRITER_H

#include <libxml/xmlwriter.h>

#include <cassert>   /* for assert() */
#include <stdexcept> /* for std::runtime_error */

namespace {
  class xml_writer {
  public:
    xml_writer(FILE* const stream) {
      assert(stream != nullptr);

      /* @see http://xmlsoft.org/html/libxml-xmlIO.html#xmlOutputBufferCreateFile */
      xmlOutputBufferPtr output = xmlOutputBufferCreateFile(stream, nullptr);
      if (output == nullptr) {
        throw std::runtime_error("xmlOutputBufferCreateFile() failed"); // TODO
      }

      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlNewTextWriter */
      _writer = xmlNewTextWriter(output);
      if (_writer == nullptr) {
        (void)xmlOutputBufferClose(output);
        throw std::runtime_error("xmlNewTextWriter() failed"); // TODO
      }
    }

    ~xml_writer() noexcept {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlFreeTextWriter */
      if (_writer != nullptr) {
        xmlFreeTextWriter(_writer);
        _writer = nullptr;
      }
    }

    void set_indent(const int indent = 2) {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterSetIndent */
      if (xmlTextWriterSetIndent(_writer, indent) == -1) {
        throw_error();
      }

      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterSetIndentString */
      const auto indent_str = reinterpret_cast<const xmlChar*>("  "); /* two spaces */
      if (xmlTextWriterSetIndentString(_writer, indent_str) == -1) {
        throw_error();
      }
    }

    void begin_document() {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterStartDocument */
      if (xmlTextWriterStartDocument(_writer, "1.0", "UTF-8", nullptr) == -1) {
        throw_error();
      }
    }

    void finish_document() {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterEndDocument */
      if (xmlTextWriterEndDocument(_writer) == -1) {
        throw_error();
      }
    }

    void write_element(const char* name, const char* text) {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterWriteElement */
      const auto name_ = reinterpret_cast<const xmlChar*>(name);
      const auto text_ = reinterpret_cast<const xmlChar*>(text);
      if (xmlTextWriterWriteElement(_writer, name_, text_) == -1) {
        throw_error();
      }
    }

    void begin_element(const char* name) {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterStartElement */
      const auto name_ = reinterpret_cast<const xmlChar*>(name);
      if (xmlTextWriterStartElement(_writer, name_) == -1) {
        throw_error();
      }
    }

    void begin_element_with_ns(const char* name, const char* ns) {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterStartElementNS */
      const auto name_ = reinterpret_cast<const xmlChar*>(name);
      const auto ns_   = reinterpret_cast<const xmlChar*>(ns);
      if (xmlTextWriterStartElementNS(_writer, nullptr, name_, ns_) == -1) {
        throw_error();
      }
    }

    void finish_element() {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterEndElement */
      //xmlTextWriterEndElement(_writer);
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterFullEndElement */
      if (xmlTextWriterFullEndElement(_writer) == -1) {
        throw_error();
      }
    }

    void write_attribute(const char* name, const char* value) {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterWriteAttribute */
      const auto name_  = reinterpret_cast<const xmlChar*>(name);
      const auto value_ = reinterpret_cast<const xmlChar*>(value);
      if (xmlTextWriterWriteAttribute(_writer, name_, value_) == -1) {
        throw_error();
      }
    }

    void write_text(const char* text) {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterWriteString */
      const auto text_ = reinterpret_cast<const xmlChar*>(text);
      if (xmlTextWriterWriteString(_writer, text_) == -1) {
        throw_error();
      }
    }

    void write_comment(const char* comment) {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterWriteComment */
      const auto comment_ = reinterpret_cast<const xmlChar*>(comment);
      if (xmlTextWriterWriteComment(_writer, comment_) == -1) {
        throw_error();
      }
    }

    void flush() {
      /* @see http://www.xmlsoft.org/html/libxml-xmlwriter.html#xmlTextWriterFlush */
      if (xmlTextWriterFlush(_writer) == -1) {
        throw_error();
      }
    }

  protected:
    void throw_error() {
      /* @see http://www.xmlsoft.org/html/libxml-xmlerror.html#xmlGetLastError */
      xmlErrorPtr error = xmlGetLastError();
      throw std::runtime_error(error->message); // TODO: use specific error class
    }

  private:
    xmlTextWriterPtr _writer = nullptr;
  };
}

#endif /* XML_WRITER_H */
