/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_READER_TRIX_H
#define RDFXX_READER_TRIX_H

#include "rdf++/reader/impl.h"

extern "C" rdf::reader::implementation* rdf_reader_for_trix(
  FILE* stream,
  const char* content_type,
  const char* charset,
  const char* base_uri);

#endif /* RDFXX_READER_TRIX_H */
