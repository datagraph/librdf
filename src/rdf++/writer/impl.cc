/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer/impl.h"
#include "rdf++/writer/jsonld.h"
#include "rdf++/writer/raptor.h"
#include "rdf++/writer/trix.h"

using namespace rdf;

writer::implementation*
writer::implementation::create(const std::string& file_path,
                               const std::string& content_type,
                               const std::string& charset,
                               const std::string& base_uri) {
  return new writer::raptor(file_path, content_type, charset, base_uri); // FIXME
}

writer::implementation*
writer::implementation::create(std::ostream& stream,
                               const std::string& content_type,
                               const std::string& charset,
                               const std::string& base_uri) {
  return new writer::raptor(stream, content_type, charset, base_uri); // FIXME
}

writer::implementation*
writer::implementation::create(FILE* const stream,
                               const std::string& content_type,
                               const std::string& charset,
                               const std::string& base_uri) {
  return new writer::raptor(stream, content_type, charset, base_uri); // FIXME
}
