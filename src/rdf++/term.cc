/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/term.h"

#include <atomic> /* for std::atomic */

using namespace rdf;

extern const term* const rdf::default_context = nullptr;

static std::atomic_size_t blank_node_id = {0};

blank_node::blank_node()
  : clonable_term<blank_node>(term_type::blank_node) {

  blank_node_id++;

  char buffer[32];
  sprintf(buffer, "g%zu", blank_node_id.load());

  string.assign(buffer);
}
