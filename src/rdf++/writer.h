/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_H
#define RDFXX_WRITER_H

#include "rdf++/term.h"
#include "rdf++/triple.h"
#include "rdf++/quad.h"

#include <cstdio>  /* for FILE */
#include <memory>  /* for std::unique_ptr */
#include <ostream> /* for std::ostream */
#include <string>  /* for std::string */

#include <boost/noncopyable.hpp>

namespace rdf {
  struct triple;
  struct quad;

  class writer : private boost::noncopyable {
    public:
      writer(
        const std::string& file_path,
        const std::string& content_type,
        const std::string& charset,
        const std::string& base_uri);

      writer(
        std::ostream& stream,
        const std::string& content_type,
        const std::string& charset,
        const std::string& base_uri);

      writer(
        FILE* stream,
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

      struct implementation;

    private:
      std::unique_ptr<implementation> _implementation;
  };
}

#endif /* RDFXX_WRITER_H */
