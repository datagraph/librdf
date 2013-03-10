/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_READER_H
#define RDFXX_READER_H

#include <memory> /* for std::unique_ptr */
#include <string> /* for std::string */

#include <boost/noncopyable.hpp>

namespace rdf {
  class reader : private boost::noncopyable {
    public:
      reader(
        const std::string& content_type,
        const std::string& charset,
        const std::string& base_uri);

      ~reader();

    private:
      class implementation;

      std::unique_ptr<implementation> _implementation;
  };
}

#endif /* RDFXX_READER_H */
