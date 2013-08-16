/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/writer/jsonld.h"

rdf::writer::implementation*
rdf_writer_for_jsonld(FILE* const stream,
                      const char* const content_type,
                      const char* const charset,
                      const char* const base_uri) {
  (void)stream, (void)content_type, (void)charset, (void)base_uri;
  return nullptr; // TODO
}
