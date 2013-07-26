/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_RAPTOR_H
#define RDFXX_RAPTOR_H

#include <istream> /* for std::istream */
#include <ostream> /* for std::ostream */

#ifdef __cplusplus
extern "C" {
#endif

#include <raptor2/raptor2.h> /* for raptor_*() */

raptor_iostream* raptor_new_iostream_from_std_istream(
  raptor_world* world,
  std::istream* stream);

raptor_iostream* raptor_new_iostream_to_std_ostream(
  raptor_world* world,
  std::ostream* stream);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RDFXX_RAPTOR_H */
