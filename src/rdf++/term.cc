/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/term.h"

#include <chrono>  /* for std::chrono */
#include <cstring> /* for std::sprintf() */
#include <random>  /* for std::random_device */

using namespace rdf;

extern const term* const rdf::default_context = nullptr;

static std::random_device random_uint;

blank_node::blank_node()
  : clonable_term<blank_node>(term_type::blank_node) {

  using clock = std::chrono::steady_clock; /* a monotonic clock */
  using precision = std::chrono::microseconds;
  using std::chrono::duration_cast;

  const auto prefix = duration_cast<precision>(clock::now().time_since_epoch()).count();
  const auto suffix = random_uint();

  char buffer[32];
  std::sprintf(buffer, "g%016zu%03u", prefix, suffix % 1000);

  string.assign(buffer);
}
