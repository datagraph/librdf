/* This is free and unencumbered software released into the public domain. */

#ifndef XML_READER_H
#define XML_READER_H

#include <libxml/xmlreader.h>

#include <cassert>   /* for assert() */
#include <map>       /* for std::map */
#include <stdexcept> /* for std::runtime_error */

namespace {
  enum class xml_node_type : unsigned int {
    none                   = XML_READER_TYPE_NONE,
    element                = XML_READER_TYPE_ELEMENT,
    attribute              = XML_READER_TYPE_ATTRIBUTE,
    text                   = XML_READER_TYPE_TEXT,
    cdata                  = XML_READER_TYPE_CDATA,
    entity_reference       = XML_READER_TYPE_ENTITY_REFERENCE,
    entity                 = XML_READER_TYPE_ENTITY,
    processing_instruction = XML_READER_TYPE_PROCESSING_INSTRUCTION,
    comment                = XML_READER_TYPE_COMMENT,
    document               = XML_READER_TYPE_DOCUMENT,
    document_type          = XML_READER_TYPE_DOCUMENT_TYPE,
    document_fragment      = XML_READER_TYPE_DOCUMENT_FRAGMENT,
    notation               = XML_READER_TYPE_NOTATION,
    whitespace             = XML_READER_TYPE_WHITESPACE,
    significant_whitespace = XML_READER_TYPE_SIGNIFICANT_WHITESPACE,
    end_element            = XML_READER_TYPE_END_ELEMENT,
    end_entity             = XML_READER_TYPE_END_ENTITY,
    xml_declaration        = XML_READER_TYPE_XML_DECLARATION,
  };

  class xml_reader {
  public:
    /**
     * Default constructor.
     */
    xml_reader() = delete;

    /**
     * Constructor.
     */
    xml_reader(FILE* const stream,
               const char* const base_uri = nullptr,
               const char* const encoding = nullptr) {
      assert(stream != nullptr);
      assert(fileno(stream) >= 0);

      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlReaderForFd */
      _reader = xmlReaderForFd(fileno(stream), base_uri, encoding, XML_PARSE_NONET);
      if (_reader == nullptr) {
        throw_error();
      }
    }

    /**
     * Copy constructor.
     */
    xml_reader(const xml_reader& other) = delete;

    /**
     * Destructor.
     */
    ~xml_reader() noexcept {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlFreeTextReader */
      if (_reader != nullptr) {
        xmlFreeTextReader(_reader);
        _reader = nullptr;
      }
    }

    /**
     * Copy assignment operator.
     */
    xml_reader& operator=(const xml_reader& other) = delete;

    /**
     * Returns the base URI of the node.
     */
    const char* base_uri() const noexcept {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderConstBaseUri */
      return string_or_nullptr(xmlTextReaderConstBaseUri(_reader));
    }

    /**
     * Returns the encoding of the document being read.
     */
    const char* encoding() const {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderConstEncoding */
      return string_or_error(xmlTextReaderConstEncoding(_reader));
    }

    /**
     * Returns the local name of the node.
     */
    const char* local_name() const noexcept {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderConstLocalName */
      return string_or_nullptr(xmlTextReaderConstLocalName(_reader));
    }

    /**
     * Returns the fully-qualified name of the node.
     */
    const char* name() const noexcept {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderConstName */
      return string_or_nullptr(xmlTextReaderConstName(_reader));
    }

    /**
     * Returns the namespace URI associated with the node.
     */
    const char* namespace_uri() const noexcept {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderConstNamespaceUri */
      return string_or_nullptr(xmlTextReaderConstNamespaceUri(_reader));
    }

    /**
     * Returns the namespace prefix associated with the node.
     */
    const char* prefix() const noexcept {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderConstPrefix */
      return string_or_nullptr(xmlTextReaderConstPrefix(_reader));
    }

    /**
     * Returns the text value of the node, if present.
     */
    const char* value() const noexcept {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderConstValue */
      return string_or_nullptr(xmlTextReaderConstValue(_reader));
    }

    /**
     * Returns the xml:lang scope of the node.
     */
    const char* xml_lang() const noexcept {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderConstXmlLang */
      return string_or_nullptr(xmlTextReaderConstXmlLang(_reader));
    }

    /**
     * Returns the depth of the node in the tree.
     */
    unsigned int depth() const {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderDepth */
      return integer_or_error(xmlTextReaderDepth(_reader));
    }

    /**
     * Retrieves all attributes of the current node.
     */
    std::map<std::string, std::string> attributes() {
      std::map<std::string, std::string> result;

      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderAttributeCount */
      const int count = xmlTextReaderAttributeCount(_reader);

      for (int no = 0; no < count; no++) {
        /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderMoveToAttributeNo */
        boolean_or_error(xmlTextReaderMoveToAttributeNo(_reader, no));

        result[name()] = value();
      }

      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderMoveToElement */
      boolean_or_error(xmlTextReaderMoveToElement(_reader));

      return result;
    }

    /**
     * Checks whether the current node has attributes.
     */
    bool has_attributes() const {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderHasAttributes */
      return boolean_or_error(xmlTextReaderHasAttributes(_reader));
    }

    /**
     * Checks whether the current node can have a text value.
     */
    bool has_value() const {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderHasValue */
      return boolean_or_error(xmlTextReaderHasValue(_reader));
    }

    /**
     * Checks whether the current node is empty.
     */
    bool is_empty() const {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderIsEmptyElement */
      return boolean_or_error(xmlTextReaderIsEmptyElement(_reader));
    }

    /**
     * Retrieves the validity status from the underlying parser.
     */
    bool is_valid() const {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderIsValid */
      return boolean_or_error(xmlTextReaderIsValid(_reader));
    }

    /**
     * Moves to the next node in the document.
     */
    bool read() {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderRead */
      return boolean_or_error(xmlTextReaderRead(_reader));
    }

    /**
     * Skips to the next node in document order, avoiding the subtree if any.
     */
    bool next() {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderNext */
      /* @see http://mail.gnome.org/archives/xml/2006-November/msg00034.html */
      return boolean_or_error(xmlTextReaderNext(_reader));
    }

    /**
     * Skips to the next node in document order, avoiding the subtree if any.
     */
    bool next_sibling() {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderNextSibling */
      /* @see http://mail.gnome.org/archives/xml/2006-November/msg00034.html */
      return boolean_or_error(xmlTextReaderNextSibling(_reader));
    }

    /**
     * Returns the type of the current node.
     */
    xml_node_type node_type() const {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderNodeType */
      return static_cast<xml_node_type>(integer_or_error(xmlTextReaderNodeType(_reader)));
    }

    /**
     * Reads the contents of an element or text node as a string.
     *
     * @note The returned string (if not `nullptr`) is owned by the caller.
     */
    char* read_string() const noexcept {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderReadString */
      return reinterpret_cast<char*>(xmlTextReaderReadString(_reader));
    }

    /**
     * Determines whether this reader has been closed.
     */
    bool is_closed() const {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderReadState */
      return integer_or_error(xmlTextReaderReadState(_reader)) ==
        XML_TEXTREADER_MODE_CLOSED;
    }

    /**
     * Closes this reader instance, precluding further operations.
     */
    void close() {
      /* @see http://xmlsoft.org/html/libxml-xmlreader.html#xmlTextReaderClose */
      if (xmlTextReaderClose(_reader) == -1) {
        throw_error();
      }
    }

  protected:
    inline bool boolean_or_error(const int value) const {
      switch (value) {
        case 1: return true;
        case 0: return false;
        case -1: default: return throw_error(), false;
      }
    }

    inline int integer_or_error(const int value) const {
      if (value == -1) throw_error();
      return value;
    }

    inline const char* string_or_nullptr(const xmlChar* const value) const noexcept {
      return reinterpret_cast<const char*>(value);
    }

    inline const char* string_or_error(const xmlChar* const value) const {
      if (value == nullptr) throw_error();
      return reinterpret_cast<const char*>(value);
    }

    void throw_error() const {
      /* @see http://www.xmlsoft.org/html/libxml-xmlerror.html#xmlGetLastError */
      xmlErrorPtr error = xmlGetLastError();
      assert(error != nullptr);
      throw std::runtime_error(error->message); // TODO: use specific error class
    }

  private:
    xmlTextReaderPtr _reader = nullptr;
  };
}

#endif /* XML_READER_H */
