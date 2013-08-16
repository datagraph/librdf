/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_TRIX_H
#define RDFXX_WRITER_TRIX_H

#include "rdf++/writer/impl.h"

extern "C" rdf::writer::implementation* rdf_writer_for_trix(
  FILE* stream,
  const char* content_type,
  const char* charset,
  const char* base_uri);

#endif /* RDFXX_WRITER_TRIX_H */
