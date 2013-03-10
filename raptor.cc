/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/raptor.h"

#include <cassert> /* for assert() */
#include <cstddef> /* for std::size_t */
#include <ostream> /* for std::ostream */

#include <raptor2/raptor2.h> /* for raptor_*() */

/**
 * @see http://librdf.org/raptor/api/raptor2-section-iostream.html#raptor-iostream-write-byte-func
 */
static int
std_iostream_write_byte(void* const user_data,
                        const int byte) {
  std::ostream* const stream = reinterpret_cast<std::ostream*>(user_data);
  assert(stream != nullptr);
  try {
    stream->put(static_cast<char>(byte));
    return 1; /* success */
  }
  catch (const std::ios_base::failure& error) {
    return 0; /* failure */
  }
}

/**
 * http://librdf.org/raptor/api/raptor2-section-iostream.html#raptor-iostream-write-bytes-func
 */
static int
std_iostream_write_bytes(void* const restrict user_data,
                         const void* const restrict data,
                         const std::size_t size,
                         const std::size_t nmemb) {
  std::ostream* const stream = reinterpret_cast<std::ostream*>(user_data);
  assert(stream != nullptr);
  assert(data != nullptr);
  try {
    const std::size_t byte_count = nmemb * size;
    stream->write(reinterpret_cast<const char*>(data), byte_count);
    return static_cast<int>(byte_count); /* success */
  }
  catch (const std::ios_base::failure& error) {
    return 0; /* failure */
  }
}

/**
 * @see http://librdf.org/raptor/api/raptor2-section-iostream.html#raptor-iostream-handler
 */
static const raptor_iostream_handler std_iostream_handler = {
  /* .version     = */ 2,
  /* .init        = */ nullptr,
  /* .finish      = */ nullptr,
  /* .write_byte  = */ std_iostream_write_byte,
  /* .write_bytes = */ std_iostream_write_bytes,
  /* .write_end   = */ nullptr,
  /* .read_bytes  = */ nullptr,
  /* .read_eof    = */ nullptr
};

raptor_iostream*
raptor_new_iostream_from_std_ostream(raptor_world* world,
                                     std::ostream* ostream) {
  return raptor_new_iostream_from_handler(world, ostream, &std_iostream_handler);
}
