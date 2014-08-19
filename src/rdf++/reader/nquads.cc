/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nquads.h"
#include "../quad.h"
#include "../term.h"
#include "../triple.h"

#include <cassert>   /* for assert() */
#include <cstdio>    /* for std::fgets() */
#include <cctype>    /* for std::is*() */
#include <cstring>   /* for std::strncpy() */
#include <stdexcept> /* for std::invalid_argument */

#include <rfc/utf8.h>

#define BUFFER_SIZE (1024UL*1024UL) /* 1 MiB */

using namespace rfc3629; /* for UTF-8 */

namespace {
  enum class line_type {
    ntriples = 3,
    nquads   = 4,
  };

  class implementation final : public rdf::reader::implementation {
    FILE* _stream {nullptr};
    std::size_t _line {0};
    const char* _input {nullptr};
    std::array<char, BUFFER_SIZE> _buffer;

  public:
    implementation(FILE* stream,
      const char* content_type,
      const char* charset,
      const char* base_uri);
    virtual ~implementation() noexcept override;
    virtual void read_triples(std::function<void (std::unique_ptr<rdf::triple>)> callback) override;
    virtual void read_quads(std::function<void (std::unique_ptr<rdf::quad>)> callback) override;
    virtual void abort() override;

  protected:
    void skip_line();
    bool read_line(line_type term_count, std::unique_ptr<rdf::term> terms[]);
    std::size_t read_term(std::unique_ptr<rdf::term>& term);
    std::size_t read_uri_reference(std::unique_ptr<rdf::term>& term);
    std::size_t read_blank_node(std::unique_ptr<rdf::term>& term);
    std::size_t read_literal(std::unique_ptr<rdf::term>& term);
    std::size_t read_escaped_string(const char* _input, const char leading_delim, const char trailing_delim, const std::size_t min_length, std::string& result);
  };
}

rdf::reader::implementation*
rdf_reader_for_nquads(FILE* const stream,
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

implementation::~implementation() noexcept {};

void
implementation::skip_line() {
  char c;
  while ((c = *_input) != '\0' && !(c == '\n' || c == '\r')) {
    _input++; /* skip the rest of the current line */
  }
}

std::size_t
implementation::read_term(std::unique_ptr<rdf::term>& term) {
  switch (*_input) {
    case '<': /* URI reference */
      return read_uri_reference(term);

    case '_': /* blank node */
      return read_blank_node(term);

    case '"': /* literal */
      return read_literal(term);

    default:  /* (unreachable) */
      assert(false);
      return 0;
  }
}

std::size_t
implementation::read_uri_reference(std::unique_ptr<rdf::term>& term) {
  const char* input = _input;

  std::string string;
  input += read_escaped_string(_input, '<', '>', 1, string);

  if (string.empty()) {
    throw std::invalid_argument{"empty URI reference"};
  }

  term.reset(new rdf::uri_reference{string});

  return input - _input;
}

std::size_t
implementation::read_blank_node(std::unique_ptr<rdf::term>& term) {
  const char* input = _input;

  if (*input++ != '_') { /* skip the leading '_' */
    throw std::invalid_argument{"expected leading '_'"};
  }

  if (*input++ != ':') { /* skip the leading ':' */
    throw std::invalid_argument{"expected leading '_:'"};
  }

  const char* label = input; /* the start of the label */

  char c;
  while ((c = *input) != '\0' && !std::isspace(c)) { // TODO: restrict this per the grammar
    input++;
  }

  const std::size_t length = input - label;
  if (length == 0) {
    throw std::invalid_argument{"empty blank node label"};
  }

  char buffer[length + 1];
  std::strncpy(buffer, label, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  term.reset(new rdf::blank_node{buffer});

  return input - _input;
}

std::size_t
implementation::read_literal(std::unique_ptr<rdf::term>& term) {
  const char* input = _input;

  std::string string;
  input += read_escaped_string(_input, '"', '"', 0, string);

  // @see http://www.w3.org/TR/n-quads/#grammar-production-literal
  switch (*input) {
    case '@': { /* a language-tagged literal */
      input++;  /* skip the '@' character */
      const char* language = input;

      while (std::isalpha(*input)) { // [a-zA-Z]+
        input++;
      }

      if (input == language) {
        throw std::invalid_argument{"invalid language tag"};
      }

      while (*input == '-') {  // ('-' [a-zA-Z0-9]+)*
        input++;
        while (std::isalnum(*input)) {
          input++;
        }
      }

      const std::size_t length = input - language;
      if (length >= 42) { // @see http://tools.ietf.org/html/rfc5646#section-4.4.1
        throw std::invalid_argument{"oversized language tag"};
      }

      char buffer[length + 1];
      std::strncpy(buffer, language, sizeof(buffer) - 1);
      buffer[sizeof(buffer) - 1] = '\0';

      term.reset(new rdf::plain_literal{string.c_str(), buffer});
      break;
    }

    case '^': { /* a datatyped literal */
      input++;               /* skip the first '^' character */
      if (*input++ != '^') { /* skip the second '^' character */
        throw std::invalid_argument{"invalid datatype specifier"};
      }

      std::string datatype_uri;
      input += read_escaped_string(input, '<', '>', 1, datatype_uri);

      term.reset(new rdf::typed_literal{string, datatype_uri});
      break;
    }

    default:  { /* a plain literal */
      term.reset(new rdf::plain_literal{string});
      break;
    }
  }

  return input - _input;
}

std::size_t
implementation::read_escaped_string(const char* _input,
                                    const char leading_delim,
                                    const char trailing_delim,
                                    const std::size_t min_length,
                                    std::string& result) {
  const char* input = _input;

  if (*input++ != leading_delim) { /* skip the leading delimiter */
    throw std::invalid_argument{"expected leading delimiter"};
  }

  for (;;) {
    char c = *input++;

    if (c == '\0') {
      throw std::invalid_argument{"unterminated term literal"};
    }
    else if (c == trailing_delim) {
      if (result.size() < min_length) {
        throw std::invalid_argument{"term is shorter than minimum length"};
      }
      break;
    }
    else if (c == '\\') {
      switch (c = *input++) {
        case 't':  result.push_back('\t'); break;
        case 'n':  result.push_back('\n'); break;
        case 'r':  result.push_back('\r'); break;
        case '"':  result.push_back('"');  break;
        case '\\': result.push_back('\\'); break;
        case 'u':
        case 'U': {
          char ubuf[9];
          const auto ulen = (c == 'u') ? 4U : 8U;
          for (auto i = 0U; i < ulen; i++) {
            if (!std::isxdigit(ubuf[i] = *input++)) {
              throw std::invalid_argument{"invalid \\u or \\U escape sequence"};
            }
          }
          ubuf[ulen] = '\0';
          long u;
          if ((u = std::strtol(ubuf, nullptr, 16)) == 0) {
            throw std::invalid_argument{"invalid \\u or \\U escape sequence"};
          }
          char buffer[5] = {0, 0, 0, 0, 0};
          utf8_encode(u, buffer); // FIXME?
          result.append(buffer);
          break;
        }
        default:
          throw std::invalid_argument{"invalid escape character"};
      }
    }
    else if (true) { // TODO: grammar restrictions
      result.push_back(c);
    }
    else {
      throw std::invalid_argument{"invalid character"};
    }
  }

  return input - _input;
}

bool
implementation::read_line(const line_type term_count,
                          std::unique_ptr<rdf::term> terms[]) {
  char c;

read_next_line:
  assert(_stream != nullptr);
  if (!std::fgets(_buffer.data(), _buffer.size(), _stream)) {
    return false;
  }
  _line++;
  _input = _buffer.data();

skip_whitespace:
  for (;;) {
    switch ((c = *_input)) {
      case '\t': case ' ': _input++; break;
      case '\r': case '\n': goto read_next_line; /* EOL */
      case '#':  goto read_next_line; /* comment */
      case '\0': goto read_next_line; /* EOS */
      default:   goto read_terms;
    }
  }

read_terms:
  std::unique_ptr<rdf::term> term;
  auto term_index = 0U;
  while (term_index < static_cast<decltype(term_index)>(term_count)) {
    switch ((c = *_input)) {
      case '<':    /* URI reference */
      case '_':    /* blank node */
      case '"': {  /* literal */
        const auto term_length = read_term(term);
        _input += term_length;
        terms[term_index].reset(term.release());
        term_index++;
        break;
      }
      case ' ':    /* whitespace */
      case '\t': {
        _input++;
        break;
      }
      case '#': {  /* comment */
        skip_line();
        goto read_next_line;
      }
      case '.': {
        if (term_index >= 3) {
          goto end_of_statement;
        }
        throw std::invalid_argument{"premature end of statement"};
      }
      default: {
        throw std::invalid_argument{"invalid input"};
      }
    }
  }

end_of_statement:
  for (;;) {
    switch ((c = *_input)) {
      case '\t': case ' ': _input++; break;
      case '.': _input++; goto end_of_line;
      default: throw std::invalid_argument{"expected end of statement"};
    }
  }

end_of_line:
  // @see http://www.w3.org/TR/n-quads/#grammar-production-EOL
  for (;;) {
    switch ((c = *_input)) {
      case '\t': case ' ':  _input++; break;
      case '\r': case '\n': _input++; goto end_of_input; // TODO: handle CRLFs correctly.
      case '#': skip_line(); break;
      default: throw std::invalid_argument{"expected end of line"};
    }
  }

end_of_input:
  return true;
}

void
implementation::read_triples(std::function<void (std::unique_ptr<rdf::triple>)> callback) {
  std::unique_ptr<rdf::term> terms[3] {nullptr, nullptr, nullptr};

  while (read_line(line_type::ntriples, terms)) {
    auto triple = new rdf::triple{*terms[0].get(), *terms[1].get(), *terms[2].get()}; /* clones all terms */
    callback(std::unique_ptr<rdf::triple>{triple});
  }
}

void
implementation::read_quads(std::function<void (std::unique_ptr<rdf::quad>)> callback) {
  std::unique_ptr<rdf::term> terms[4] {nullptr, nullptr, nullptr, nullptr};

  while (read_line(line_type::nquads, terms)) {
    auto quad = terms[3] ? /* clones all terms */
      new rdf::quad{*terms[0].get(), *terms[1].get(), *terms[2].get(), *terms[3].get()} :
      new rdf::quad{*terms[0].get(), *terms[1].get(), *terms[2].get()};
    callback(std::unique_ptr<rdf::quad>{quad});
  }
}

void
implementation::abort() {}
