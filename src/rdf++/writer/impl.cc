/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/format.h"
#include "rdf++/writer/impl.h"
#include "rdf++/writer/jsonld.h"
#include "rdf++/writer/raptor.h"
#include "rdf++/writer/trix.h"

#include <cstring> /* for std::strcmp() */

using namespace rdf;

writer::implementation*
writer::implementation::create(const std::string& file_path,
                               const std::string& content_type,
                               const std::string& charset,
                               const std::string& base_uri) {
  const format* const format = format::find_for_content_type(content_type);
  if (format == nullptr) {
    throw std::invalid_argument("unknown content type: " + content_type);
  }

  if (std::strcmp("raptor", format->module_name) == 0) {
    return new writer::raptor(file_path, content_type, charset, base_uri);
  }

  if (std::strcmp("jsonld", format->module_name) == 0) {
    //return new writer::jsonld(file_path, content_type, charset, base_uri);
  }

  if (std::strcmp("trix", format->module_name) == 0) {
    return new writer::trix(file_path, content_type, charset, base_uri);
  }

  throw std::invalid_argument("unknown content type: " + content_type);
}

writer::implementation*
writer::implementation::create(std::ostream& stream,
                               const std::string& content_type,
                               const std::string& charset,
                               const std::string& base_uri) {
  const format* const format = format::find_for_content_type(content_type);
  if (format == nullptr) {
    throw std::invalid_argument("unknown content type: " + content_type);
  }

  if (std::strcmp("raptor", format->module_name) == 0) {
    return new writer::raptor(stream, content_type, charset, base_uri);
  }

  if (std::strcmp("jsonld", format->module_name) == 0) {
    //return new writer::jsonld(stream, content_type, charset, base_uri);
  }

  if (std::strcmp("trix", format->module_name) == 0) {
    return new writer::trix(stream, content_type, charset, base_uri);
  }

  throw std::invalid_argument("unknown content type: " + content_type);
}

writer::implementation*
writer::implementation::create(FILE* const stream,
                               const std::string& content_type,
                               const std::string& charset,
                               const std::string& base_uri) {
  const format* const format = format::find_for_content_type(content_type);
  if (format == nullptr) {
    throw std::invalid_argument("unknown content type: " + content_type);
  }

  if (std::strcmp("raptor", format->module_name) == 0) {
    return new writer::raptor(stream, content_type, charset, base_uri);
  }

  if (std::strcmp("jsonld", format->module_name) == 0) {
    //return new writer::jsonld(stream, content_type, charset, base_uri);
  }

  if (std::strcmp("trix", format->module_name) == 0) {
    return new writer::trix(stream, content_type, charset, base_uri);
  }

  throw std::invalid_argument("unknown content type: " + content_type);
}
