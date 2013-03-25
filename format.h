/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_FORMAT_H
#define RDFXX_FORMAT_H

#include <string> /* for std::string */

namespace rdf {
  namespace format {
    inline bool supported(const std::string& content_type) {
      return supported(content_type.c_str());
    }

    bool supported(const char* content_type);

    const char* find_writer_name_for(const char* content_type);
  }
}

#endif /* RDFXX_FORMAT_H */
