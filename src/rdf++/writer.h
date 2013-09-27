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

      void configure(const std::string& key,
                     const std::string& value) {
        configure(key.c_str(), value.c_str());
      }

      /**
       * @throws std::invalid_argument if `key` is `nullptr` or an empty string
       */
      void configure(const char* key, const char* value);

      void define_prefix(const std::string& prefix,
                         const std::string& uri_string);

      void begin();

      void finish();

      void flush();

      void write_triple(const triple& triple);

      void write_quad(const quad& quad);

      void write_comment(const std::string& comment) {
        write_comment(comment.c_str());
      }

      void write_comment(const char* comment);

      struct implementation;

    private:
      std::unique_ptr<implementation> _implementation;
  };
}

#endif /* RDFXX_WRITER_H */
