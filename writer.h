/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_H
#define RDFXX_WRITER_H

#include <memory> /* for std::unique_ptr */
#include <string> /* for std::string */

#include <boost/noncopyable.hpp>

namespace rdf {
  class writer : private boost::noncopyable {
    public:
      writer(
        const std::string& content_type,
        const std::string& charset,
        const std::string& base_uri);

      ~writer();

    private:
      class implementation;

      std::unique_ptr<implementation> _implementation;
  };
}

#endif /* RDFXX_WRITER_H */
