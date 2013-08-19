/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_READER_H
#define RDFXX_READER_H

#include <cstdio>     /* for FILE */
#include <functional> /* for std::function */
#include <istream>    /* for std::istream */
#include <memory>     /* for std::unique_ptr */
#include <string>     /* for std::string */

#include <boost/noncopyable.hpp>

namespace rdf {
  struct triple;
  struct quad;
  class reader;
}

class rdf::reader : private boost::noncopyable {
public:
  reader(
    const std::string& file_path,
    const std::string& content_type,
    const std::string& charset,
    const std::string& base_uri);

  reader(
    std::istream& stream,
    const std::string& content_type,
    const std::string& charset,
    const std::string& base_uri);

  reader(
    FILE* stream,
    const std::string& content_type,
    const std::string& charset,
    const std::string& base_uri);

  ~reader();

  void read_triples(std::function<void (rdf::triple*)> callback);

  void read_quads(std::function<void (rdf::quad*)> callback);

  void abort();

  struct implementation;

private:
  std::unique_ptr<implementation> _implementation;
};

#endif /* RDFXX_READER_H */
