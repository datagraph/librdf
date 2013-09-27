/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/format.h"

#include <cassert> /* for assert() */
#include <cstring> /* for std::strcmp() */

using namespace rdf;

/* @see http://librdf.org/raptor/api/raptor-formats-types-by-serializer.html */
static const format rdf_format_info[] = {
#ifndef DISABLE_NQUADS
  /* N-Triples (.nt) */
  {"application/n-triples", "UTF-8", "nt",     "raptor", "ntriples", "ntriples"},      /* official */
  {"text/plain",            "ASCII", nullptr,  "raptor", "ntriples", "ntriples"},
  {"text/ntriples",         nullptr, nullptr,  "raptor", "ntriples", "ntriples"},      /* unofficial, by analogy with Turtle */
  {"text/x-ntriples",       nullptr, nullptr,  "raptor", "ntriples", "ntriples"},      /* unofficial, by analogy with N-Quads */
  /* N-Quads (.nq) */
  {"application/n-quads",   "UTF-8", "nq",     "raptor", "nquads", "nquads"},          /* official */
  {"text/nquads",           nullptr, nullptr,  "raptor", "nquads", "nquads"},          /* unofficial, by analogy with Turtle */
  {"text/x-nquads",         nullptr, nullptr,  "raptor", "nquads", "nquads"},
#endif
#ifndef DISABLE_TURTLE
  /* Turtle (.ttl) */
  {"text/turtle",           nullptr, "ttl",    "raptor", "turtle", "turtle"},          /* official */
  {"application/turtle",    nullptr, nullptr,  "raptor", "turtle", "turtle"},
  {"application/x-turtle",  nullptr, nullptr,  "raptor", "turtle", "turtle"},          /* unofficial, deprecated */
  /* Notation3 (.n3) */
  {"text/n3",               nullptr, "n3",     "raptor", "turtle", "turtle"},          /* official */
  {"text/rdf+n3",           nullptr, nullptr,  "raptor", "turtle", "turtle"},
  {"application/rdf+n3",    nullptr, nullptr,  "raptor", "turtle", "turtle"},
#endif
#ifndef DISABLE_TRIG
  {"application/trig",      nullptr, "trig",   "raptor", "trig",   nullptr},
#endif
#ifndef DISABLE_TRIX
  {"application/trix",      nullptr, "trix",   "trix",   nullptr,  nullptr},
#endif
#ifndef DISABLE_RDFXML
  /* RDF/XML (.rdf) */
#if 0
  {"application/rdf+xml",   nullptr, "rdf",    "raptor", "rdfxml", "rdfxml",},
#else
  {"application/rdf+xml",   nullptr, "rdf",    "raptor", "rdfxml", "rdfxml-abbrev",},
#endif
#endif
  /* RDF/JSON (.json) */
  {"application/json",      nullptr, "json",   "raptor", "json", "json"},
  {"text/json",             nullptr, nullptr,  "raptor", "json", "json"},              /* unofficial, deprecated */
#ifndef DISABLE_JSONLD
  /* JSON-LD (.jsonld) */
  {"application/ld+json",   nullptr, "jsonld", "jsonld", nullptr, nullptr},
#endif
#ifndef DISABLE_XSLT
  {"application/xml",       nullptr, "xml",    "xslt",   nullptr,  nullptr},
#endif
#if 1
  /* Graphviz (.dot) */
  {"text/x-graphviz",       nullptr, "dot",    "raptor", nullptr, "dot"},
  /* HTML/XHTML (.html) */
  {"application/xhtml+xml", nullptr, nullptr,  "raptor", "rdfa", "html"},              /* XHTML 1.0 */
  {"text/html",             nullptr, nullptr,  "raptor", "rdfa", "html"},              /* HTML5 */
  /* RSS 1.0 (.rss) */
  {"application/rss+xml",   nullptr, nullptr,  "raptor", "rss-tag-soup", "rss-1.0"},
  /* Atom (.xml) */
  {"application/atom+xml",  nullptr, nullptr,  "raptor", "rss-tag-soup", "atom"},
#endif
  {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
};

static const unsigned int rdf_format_count =
  (sizeof(rdf_format_info) / sizeof(rdf_format_info[0])) - 1;

bool
format::supported(const char* const content_type) {
  if (content_type != nullptr) {
    for (unsigned int i = 0; i < rdf_format_count; i++) {
      const format* const format_info = &rdf_format_info[i];
      assert(format_info->content_type != nullptr);

      if (std::strcmp(content_type, format_info->content_type) == 0) {
        return true; /* found */
      }
    }
  }

  return false; /* not found */
}

const format*
format::find_for_content_type(const char* const content_type) {
  assert(content_type != nullptr);

  for (unsigned int i = 0; i < rdf_format_count; i++) {
    const format* const format_info = &rdf_format_info[i];
    assert(format_info->content_type != nullptr);

    if (std::strcmp(content_type, format_info->content_type) == 0) {
      return format_info;
    }
  }

  return nullptr; /* not found */
}
