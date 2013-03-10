/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_H
#define RDFXX_WRITER_H

#include <ostream> /* for std::ostream */
#include <memory>  /* for std::unique_ptr */
#include <string>  /* for std::string */

#include <boost/noncopyable.hpp>

namespace rdf {
  class triple;
  class quad;

  class writer : private boost::noncopyable {
    public:
      writer(
        std::ostream& stream,
        const std::string& content_type,
        const std::string& charset,
        const std::string& base_uri);

      ~writer();

      void define_prefix(
        const std::string& prefix,
        const std::string& uri_string);

      void begin();

      void finish();

      void flush();

      void write_triple(const triple& triple);

      void write_quad(const quad& quad);

    private:
      class implementation;

      std::unique_ptr<implementation> _implementation;
  };
}

#endif /* RDFXX_WRITER_H */
