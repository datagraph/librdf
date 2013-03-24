/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/format.h"

#include <cassert> /* for assert() */
#include <cstring> /* for std::strcmp() */

using namespace rdf;

typedef struct rdf_format {
  const char* const content_type;
  const char* const charset;
  const char* const file_extension;
  const char* const parser_name;
  const char* const serializer_name;
} rdf_format_t;

/* @see http://librdf.org/raptor/api/raptor-formats-types-by-serializer.html */
static const rdf_format_t rdf_format_info[] = {
  /* N-Triples (.nt) */
  {"text/plain",            nullptr, "nt",   "ntriples", "ntriples"},
  {"text/ntriples",         nullptr, nullptr, "ntriples", "ntriples"},      /* unofficial, by analogy with Turtle */
  {"text/x-ntriples",       nullptr, nullptr, "ntriples", "ntriples"},      /* unofficial, by analogy with N-Quads */
  /* N-Quads (.nq) */
  {"text/nquads",           nullptr, nullptr, "nquads", "nquads"},          /* unofficial, by analogy with Turtle */
  {"text/x-nquads",         nullptr, "nq",    "nquads", "nquads"},
  /* Turtle (.ttl) */
  {"text/turtle",           nullptr, "ttl",   "turtle", "turtle"},          /* official */
  {"application/turtle",    nullptr, nullptr, "turtle", "turtle"},
  {"application/x-turtle",  nullptr, nullptr, "turtle", "turtle"},          /* unofficial, deprecated */
  /* Notation3 (.n3) */
  {"text/n3",               nullptr, "n3",    "turtle", "turtle"},          /* official */
  {"text/rdf+n3",           nullptr, nullptr, "turtle", "turtle"},
  {"application/rdf+n3",    nullptr, nullptr, "turtle", "turtle"},
  /* RDF/XML (.rdf) */
#if 1
  {"application/rdf+xml",   nullptr, "rdf",   "rdfxml", "rdfxml",},
#else
  {"application/rdf+xml",   nullptr, "rdf",   "rdfxml", "rdfxml-abbrev",},
#endif
  /* RDF/JSON (.json) */
  {"application/json",      nullptr, "json",  "json", "json"},
  {"text/json",             nullptr, nullptr, "json", "json"},              /* unofficial, deprecated */
  /* Graphviz (.dot) */
  {"text/x-graphviz",       nullptr, "dot",   nullptr, "dot"},
  /* HTML/XHTML (.html) */
  {"application/xhtml+xml", nullptr, nullptr, "rdfa", "html"},              /* XHTML 1.0 */
  {"text/html",             nullptr, nullptr, "rdfa", "html"},              /* HTML5 */
  /* RSS 1.0 (.rss) */
  {"application/rss+xml",   nullptr, nullptr, "rss-tag-soup", "rss-1.0"},
  /* Atom (.xml) */
  {"application/atom+xml",  nullptr, nullptr, "rss-tag-soup", "atom"},
  {nullptr, nullptr, nullptr, nullptr, nullptr},
};

static const unsigned int rdf_format_count =
  (sizeof(rdf_format_info) / sizeof(rdf_format_info[0])) - 1;

const char*
format::find_writer_name_for(const char* content_type) {
  assert(content_type != nullptr);

  for (unsigned int i = 0; i < rdf_format_count; i++) {
    const rdf_format_t* const format_info = &rdf_format_info[i];
    if (std::strcmp(content_type, format_info->content_type) == 0) {
      return format_info->serializer_name; /* found */
    }
  }

  return nullptr; /* not found */
}
