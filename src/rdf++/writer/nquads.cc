/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nquads.h"
#include "../quad.h"
#include "../term.h"
#include "../triple.h"

#include <cassert> /* for assert() */
#include <cstdio>  /* for FILE, std::f*() */

#include <rfc/utf8.h>

using namespace rfc3629; /* for UTF-8 */

namespace {
  class implementation final : public rdf::writer::implementation {
    FILE* _stream {nullptr};

  public:
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
    void write_escaped_iriref(const char* string);
    void write_escaped_string(const char* string);
  };
}

rdf::writer::implementation*
rdf_writer_for_nquads(FILE* const stream,
                      const char* const content_type,
                      const char* const charset,
                      const char* const base_uri) {
  return new implementation(stream, content_type, charset, base_uri);
}

implementation::implementation(FILE* const stream,
                               const char* const content_type,
                               const char* const charset,
                               const char* const base_uri)
  : _stream{stream} {
  assert(stream != nullptr);
  static_cast<void>(content_type); /* unused */
  static_cast<void>(charset);      /* unused */
  static_cast<void>(base_uri);     /* unused */
}

implementation::~implementation() noexcept {}

void
implementation::configure(const char* const key,
                          const char* const value) {
  static_cast<void>(key);   /* unused */
  static_cast<void>(value); /* unused */
}

void
implementation::begin() {}

void
implementation::finish() {}

void
implementation::write_triple(const rdf::triple& triple) {
  assert(triple.subject);
  write_term(*triple.subject);
  std::fputc(' ', _stream);

  assert(triple.predicate);
  write_term(*triple.predicate);
  std::fputc(' ', _stream);

  assert(triple.object);
  write_term(*triple.object);
  std::fputc(' ', _stream);

  std::fputs(".\n", _stream);
}

void
implementation::write_quad(const rdf::quad& quad) {
  assert(quad.subject);
  write_term(*quad.subject);
  std::fputc(' ', _stream);

  assert(quad.predicate);
  write_term(*quad.predicate);
  std::fputc(' ', _stream);

  assert(quad.object);
  write_term(*quad.object);
  std::fputc(' ', _stream);

  if (quad.context) {
    write_term(*quad.context);
    std::fputc(' ', _stream);
  }

  std::fputs(".\n", _stream);
}

void
implementation::write_comment(const char* const comment) {
  std::fprintf(_stream, "# %s\n", comment); // TODO: handle multi-line comments.
}

void
implementation::flush() {
  std::fflush(_stream);
}

void
implementation::write_term(const rdf::term& term_) {
  switch (term_.type) {
    case rdf::term_type::uri_reference: {
      const auto& term = dynamic_cast<const rdf::uri_reference&>(term_);
      std::fputc('<', _stream);
      write_escaped_iriref(term.string.c_str());
      std::fputc('>', _stream);
      break;
    }

    case rdf::term_type::blank_node: {
      const auto& term = dynamic_cast<const rdf::blank_node&>(term_);
      std::fprintf(_stream, "_:%s", term.string.c_str());
      break;
    }

    case rdf::term_type::plain_literal: {
      const auto& term = dynamic_cast<const rdf::plain_literal&>(term_);
      std::fputc('"', _stream);
      write_escaped_string(term.string.c_str());
      std::fputc('"', _stream);
      if (!term.language_tag.empty()) {
        std::fprintf(_stream, "@%s", term.language_tag.c_str());
      }
      break;
    }

    case rdf::term_type::typed_literal: {
      const auto& term = dynamic_cast<const rdf::typed_literal&>(term_);
      std::fputc('"', _stream);
      write_escaped_string(term.string.c_str());
      std::fputs("\"^^<", _stream);
      write_escaped_iriref(term.datatype_uri.c_str());
      std::fputc('>', _stream);
      break;
    }

    default: {
       assert(false && "invalid term type for #write_term");
    }
  }
}

void
implementation::write_escaped_iriref(const char* string) {
  // @see http://www.w3.org/TR/n-quads/#grammar-production-IRIREF
  char c;
  while ((c = *string++) != '\0') {
    switch (c) {
      // @see http://www.w3.org/TR/n-quads/#grammar-production-UCHAR
      case '\x00'...'\x20':
      case '<': case '>': case '"': case '{': case '}':
      case '|': case '^': case '`': case '\\':
        std::fprintf(_stream, "\\u%04X", c);
        break;
      default:
        std::fputc(c, _stream); // TODO: implement UCHAR escaping
    }
  }
}

void
implementation::write_escaped_string(const char* string) {
  // @see http://www.w3.org/TR/n-quads/#grammar-production-STRING_LITERAL_QUOTE
  char c;
  while ((c = *string++) != '\0') {
    switch (c) {
      // @see http://www.w3.org/TR/n-quads/#grammar-production-ECHAR
      case '\t': std::fputs("\\t", _stream);  break;
      case '\b': std::fputs("\\b", _stream);  break;
      case '\n': std::fputs("\\n", _stream);  break;
      case '\r': std::fputs("\\r", _stream);  break;
      case '\f': std::fputs("\\f", _stream);  break;
      case '"':  std::fputs("\\\"", _stream); break;
      //case '\'': std::fputs("\\'", _stream); /* not needed */
      case '\\': std::fputs("\\\\", _stream); break;

      // @see http://www.w3.org/TR/n-quads/#grammar-production-UCHAR
      default:
        std::fputc(c, _stream); // TODO: implement UCHAR escaping
    }
  }
}
