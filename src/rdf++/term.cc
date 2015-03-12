/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/term.h"

#include <chrono>    /* for std::chrono */
#include <cstring>   /* for std::sprintf() */
#include <random>    /* for std::random_device */
#include <stdexcept> /* for std::invalid_argument */

using namespace rdf;

////////////////////////////////////////////////////////////////////////////////

extern const term* const rdf::default_context{nullptr};

static std::random_device random_uint;

////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<term>
term::create(const term_type type,
             const char* const string,
             const char* const language_tag,
             const char* const datatype_uri) {
  std::unique_ptr<term> result;
  switch (type) {
    case term_type::uri_reference:
      result.reset(new uri_reference(string));
      break;
    case term_type::blank_node:
      result.reset(new blank_node(string));
      break;
    case term_type::plain_literal:
      result.reset(new plain_literal(string, language_tag));
      break;
    case term_type::typed_literal:
      result.reset(new typed_literal(string, datatype_uri));
      break;
    case term_type::none:
    default:
      throw std::invalid_argument("invalid term type");
  }
  return result;
}

////////////////////////////////////////////////////////////////////////////////

std::size_t
uri_reference::size() const {
  return 0; // TODO
}

////////////////////////////////////////////////////////////////////////////////

blank_node::blank_node()
  : clonable_term<blank_node>{term_type::blank_node} {

  using clock = std::chrono::steady_clock; /* a monotonic clock */
  using precision = std::chrono::microseconds;
  using std::chrono::duration_cast;

  const auto prefix = duration_cast<precision>(clock::now().time_since_epoch()).count();
  const auto suffix = random_uint();

  char buffer[32];
  std::sprintf(buffer, "g%016zu%03u", static_cast<std::size_t>(prefix), suffix % 1000);

  string.assign(buffer);
}

std::size_t
blank_node::size() const {
  return 0; // TODO
}

////////////////////////////////////////////////////////////////////////////////

std::size_t
plain_literal::size() const {
  return 0; // TODO
}

////////////////////////////////////////////////////////////////////////////////

std::size_t
typed_literal::size() const {
  return 0; // TODO
}
