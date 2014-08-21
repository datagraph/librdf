/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_READER_H
#define RDFXX_READER_H

#include <cstddef>    /* for std::size_t */
#include <cstdio>     /* for FILE */
#include <functional> /* for std::function */
#include <istream>    /* for std::istream */
#include <memory>     /* for std::unique_ptr */
#include <stdexcept>  /* for std::runtime_error */
#include <string>     /* for std::string */

namespace rdf {
  struct triple;
  struct quad;
  class reader_error;
  class reader;
}

/**
 * RDF parser error.
 */
class rdf::reader_error : public std::runtime_error {
protected:
  const std::size_t _line {0};
  const std::size_t _column {0};

public:
  reader_error(const char* const what)
    : std::runtime_error{what} {}

  reader_error(const char* const what,
               const std::size_t line,
               const std::size_t column = 0)
    : std::runtime_error{what},
      _line{line},
      _column{column} {}

  /**
   * Returns the line number, if any, that this error relates to.
   *
   * @return nonzero if a line number is indicated, zero otherwise
   */
  std::size_t line_number() const noexcept {
    return _line;
  }

  /**
   * Returns the column number, if any, that this error relates to.
   *
   * @return nonzero if a column number is indicated, zero otherwise
   */
  std::size_t column_number() const noexcept {
    return _column;
  }
};

/**
 * RDF parser.
 *
 * @note Instances of this class are movable, but not copyable.
 */
class rdf::reader {
public:
  /**
   * Default constructor.
   */
  reader() noexcept = delete;

  /**
   * Constructor.
   */
  reader(const std::string& file_path,
    const std::string& content_type,
    const std::string& charset,
    const std::string& base_uri);

  /**
   * Constructor.
   */
  reader(std::istream& stream,
    const std::string& content_type,
    const std::string& charset,
    const std::string& base_uri);

  /**
   * Constructor.
   */
  reader(FILE* stream);

  /**
   * Constructor.
   */
  reader(FILE* stream,
    const std::string& content_type,
    const std::string& charset,
    const std::string& base_uri);

  /**
   * Copy constructor.
   */
  reader(const reader& other) = delete;

  /**
   * Move constructor.
   */
  reader(reader&& other) /*noexcept*/;

  /**
   * Destructor.
   */
  ~reader() noexcept;

  /**
   * Copy assignment operator.
   */
  reader& operator=(const reader& other) = delete;

  /**
   * Move assignment operator.
   */
  reader& operator=(reader&& other) /*noexcept*/;

  void read_triples(std::function<void (std::unique_ptr<rdf::triple>)> callback);

  void read_quads(std::function<void (std::unique_ptr<rdf::quad>)> callback);

  void abort();

  struct implementation;

private:
  std::unique_ptr<implementation> _implementation;
};

#endif /* RDFXX_READER_H */
