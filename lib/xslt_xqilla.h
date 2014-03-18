/* This is free and unencumbered software released into the public domain. */

#ifndef XSLT_XQILLA_H
#define XSLT_XQILLA_H

#include <xqilla/xqilla-simple.hpp>
#include <xercesc/util/BinInputStream.hpp>

#include <cassert>   /* for assert() */
#include <cstdio>    /* for FILE, std::fflush(), std::fread(), std::ftell(), std::fwrite() */
#include <stdexcept> /* for std::runtime_error */
#include <string>    /* for std::string */

namespace {
  /**
   * @see http://xerces.apache.org/xerces-c/apiDocs-3/classBinInputStream.html
   */
  class xslt_input_stream : public xercesc::BinInputStream {
    using MemoryManager = xercesc::MemoryManager;
    using XMLPlatformUtils = xercesc::XMLPlatformUtils;

  public:
    xslt_input_stream(FILE* const stream,
                      MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager)
      : _stream(stream) {
      assert(stream != nullptr);
      (void)manager;
    }

    virtual XMLFilePos curPos() const override {
      const auto pos = std::ftell(_stream);
      if (pos == -1) {
        // TODO: error handling
      }
      return pos;
    }

    virtual XMLSize_t readBytes(XMLByte* const buffer,
                                const XMLSize_t max_bytes) override {
      return std::fread(buffer, 1, max_bytes, _stream);
    }

    virtual const XMLCh* getContentType() const override {
      return nullptr; /* not supported */
    }

  private:
    FILE* _stream = nullptr;
  };

  /**
   * @see http://xqilla.sourceforge.net/docs/simple-api/classxercesc_1_1InputSource.html
   */
  class xslt_input_source : public xercesc::InputSource {
    using MemoryManager = xercesc::MemoryManager;
    using XMLPlatformUtils = xercesc::XMLPlatformUtils;

  public:
    xslt_input_source(FILE* const stream,
                      MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager)
      : InputSource("stream", manager),
        _stream(stream) {
      assert(stream != nullptr);
    }

    xslt_input_stream* makeStream() const override {
      return new (getMemoryManager()) xslt_input_stream(_stream, getMemoryManager());
    }

  private:
    FILE* _stream = nullptr;
  };

  /**
   * @see http://xqilla.sourceforge.net/docs/simple-api/classxercesc_1_1XMLFormatTarget.html
   */
  class xslt_format_target : public xercesc::XMLFormatTarget {
    using XMLExcepts = xercesc::XMLExcepts;
    using XMLFormatter = xercesc::XMLFormatter;
    using XMLPlatformUtilsException = xercesc::XMLPlatformUtilsException;

  public:
    xslt_format_target(FILE* const stream) : _stream(stream) {
      assert(stream != nullptr);
    }

    ~xslt_format_target() { flush(); }

    virtual void writeChars(const XMLByte* const buffer,
                            const XMLSize_t count,
                            XMLFormatter* const formatter) override {
      (void)formatter;
      const XMLSize_t written = std::fwrite(buffer, sizeof(XMLByte), count, _stream);
      if (written != count) {
        ThrowXML(XMLPlatformUtilsException, // TODO: wrap error class
          XMLExcepts::File_CouldNotWriteToFile);
      }
    }

    virtual void flush() override {
      std::fflush(_stream);
    }

  private:
    FILE* _stream = nullptr;
  };
}

/**
 * XSLT error.
 *
 * @see http://xqilla.sourceforge.net/docs/simple-api/classXQException.html
 */
class xslt_error : public std::runtime_error {
public:
  xslt_error(const XQException& error)
    : std::runtime_error(UTF8(error.getError())),
      _error(error) {}

private:
  const XQException& _error;
};

/**
 * XSLT processor.
 *
 * @see http://xqilla.sourceforge.net/docs/simple-api/classXQilla.html
 */
class xslt_processor {
  static DynamicContext* create_context(const XQilla& xqilla) {
    try {
      return xqilla.createContext(XQilla::XSLT2);
    }
    catch (const XQException& error) {
      throw xslt_error(error);
    }
  }

public:
  xslt_processor() {}

  XQQuery* parse_from_url(const std::string& url) {
    return parse_from_url(url.c_str());
  }

  XQQuery* parse_from_url(const char* const url) {
    assert(url != nullptr);
    try {
      return _xqilla.parseFromURI(X(url), create_context(_xqilla));
    }
    catch (const XQException& error) {
      throw xslt_error(error);
    }
  }

private:
  XQilla _xqilla;
};

/**
 * XSLT template.
 *
 * This class is thread safe and re-entrant.
 *
 * @note The referenced xslt_processor instance must persist for the
 * lifetime of all xslt_template instances created using it.
 *
 * @see http://xqilla.sourceforge.net/docs/simple-api/classXQQuery.html
 */
class xslt_template {
  static constexpr const char* xml_encoding = "UTF-8";
  static constexpr const char* xml_version  = "1.0";

public:
  xslt_template(xslt_processor& processor,
                const std::string& url)
    : _query(processor.parse_from_url(url)) {}

  void execute(const char* const input_url,
               FILE* const output) {
    assert(input_url != nullptr);
    assert(output != nullptr);
    try {
      AutoDelete<DynamicContext> context(_query->createDynamicContext());
      XPath2MemoryManager* const mm = context->getMemoryManager();

      const Sequence seq = context->resolveDocument(X(input_url));
      if (!seq.isEmpty() && seq.first()->isNode()) {
        context->setContextItem(seq.first());
        context->setContextPosition(1);
        context->setContextSize(1);
      }

      xslt_format_target target(output);
      EventSerializer writer(xml_encoding, xml_version, &target, mm);
      writer.addNewlines(true);
      NSFixupFilter nsfilter(&writer, mm);

      _query->execute(&nsfilter, context);
    }
    catch (const XQException& error) {
      throw xslt_error(error);
    }
  }

  void execute(FILE* const input,
               FILE* const output) {
    assert(input != nullptr);
    assert(output != nullptr);
    try {
      AutoDelete<DynamicContext> context(_query->createDynamicContext());
      XPath2MemoryManager* const mm = context->getMemoryManager();

      xslt_input_source input_source(input);
      const Node::Ptr node = context->parseDocument(input_source);

      xslt_format_target target(output);
      EventSerializer writer(xml_encoding, xml_version, &target, mm);
      writer.addNewlines(true);
      NSFixupFilter nsfilter(&writer, mm);

      _query->execute(&nsfilter, node, context);
    }
    catch (const XQException& error) {
      throw xslt_error(error);
    }
  }

private:
  AutoDelete<XQQuery> _query {nullptr};
};

#endif /* XSLT_XQILLA_H */
