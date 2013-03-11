/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_READER_H
#define RDFXX_READER_H

#include <istream> /* for std::istream */
#include <memory>  /* for std::unique_ptr */
#include <string>  /* for std::string */

#include <boost/noncopyable.hpp>

namespace rdf {
  class triple;
  class quad;

  class reader : private boost::noncopyable {
    public:
      reader(
        std::istream& stream,
        const std::string& content_type,
        const std::string& charset,
        const std::string& base_uri);

      ~reader();

      std::unique_ptr<triple> read_triple();

      std::unique_ptr<quad> read_quad();

    private:
      class implementation;

      std::unique_ptr<implementation> _implementation;
  };
}

#endif /* RDFXX_READER_H */
