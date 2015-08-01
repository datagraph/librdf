/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/format.h"

#include <cassert> /* for assert() */
#include <cstring> /* for std::strcmp(), std::strrchr() */

using namespace rdf;

////////////////////////////////////////////////////////////////////////////////

/* @see http://librdf.org/raptor/api/raptor-formats-types-by-serializer.html */
static const format rdf_format_info[] = {
#ifndef DISABLE_NQUADS
  /* N-Triples (.nt) */
  {"application/n-triples", "UTF-8", "nt",     "nquads", "ntriples", "ntriples"},      /* official */
  {"text/plain",            "ASCII", nullptr,  "nquads", "ntriples", "ntriples"},
  {"text/ntriples",         nullptr, nullptr,  "nquads", "ntriples", "ntriples"},      /* unofficial, by analogy with Turtle */
  {"text/x-ntriples",       nullptr, nullptr,  "nquads", "ntriples", "ntriples"},      /* unofficial, by analogy with N-Quads */
  /* N-Quads (.nq) */
  {"application/n-quads",   "UTF-8", "nq",     "nquads", "nquads", "nquads"},          /* official */
  {"text/nquads",           nullptr, nullptr,  "nquads", "nquads", "nquads"},          /* unofficial, by analogy with Turtle */
  {"text/x-nquads",         nullptr, nullptr,  "nquads", "nquads", "nquads"},
#endif
#ifndef DISABLE_TURTLE
  /* Turtle (.ttl) */
#ifdef HAVE_LIBSERD
  {"text/turtle",           nullptr, "ttl",    "serd",   "turtle", "turtle"},          /* official */
  {"text/turtle;serd",      nullptr, nullptr,  "serd",   "turtle", "turtle"},
#endif
  {"text/turtle;raptor",    nullptr, nullptr,  "raptor", "turtle", "turtle"},
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
#ifndef DISABLE_RDFJSON
  /* RDF/JSON (.rj, .json) */
  {"application/rdf+json",  "UTF-8", "rj",     "rdfjson", nullptr, nullptr},
  {"application/json",      "UTF-8", "json",   "rdfjson", nullptr, nullptr},
  {"text/json",             "UTF-8", nullptr,  "rdfjson", nullptr, nullptr},           /* unofficial, deprecated */
#endif
#ifndef DISABLE_RDFXML
  /* RDF/XML (.rdf) */
#if 0
  {"application/rdf+xml",   nullptr, "rdf",    "raptor", "rdfxml", "rdfxml"},
#else
  {"application/rdf+xml",   nullptr, "rdf",    "raptor", "rdfxml", "rdfxml-abbrev"},
#endif
#endif
#ifndef DISABLE_JSONLD
  /* JSON-LD (.jsonld) */
  {"application/ld+json",   nullptr, "jsonld", "jsonld", nullptr, nullptr},
#endif
#ifndef DISABLE_XSLT
  {"application/xml",       nullptr, "xml",    "xslt",   nullptr,  nullptr},
#endif
#ifndef DISABLE_PAJEK
  /* Pajek (.net) */
  {"application/x-pajek",   nullptr, "net",    "pajek",  nullptr,  nullptr},
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
};

////////////////////////////////////////////////////////////////////////////////

bool
format::supported(const char* const content_type) {
  if (content_type != nullptr) {
    for (const auto& format_info : rdf_format_info) {
      assert(format_info.content_type != nullptr);
      if (std::strcmp(content_type, format_info.content_type) == 0) {
        return true; /* found */
      }
    }
  }

  return false; /* not found */
}

const format*
format::find_for_content_type(const char* const content_type) {
  assert(content_type != nullptr);

  for (const auto& format_info : rdf_format_info) {
    assert(format_info.content_type != nullptr);
    if (std::strcmp(content_type, format_info.content_type) == 0) {
      return &format_info;
    }
  }

  return nullptr; /* not found */
}

const format*
format::find_for_file_extension(const char* const file_extension) {
  assert(file_extension != nullptr);

  for (const auto& format_info : rdf_format_info) {
    if (format_info.file_extension &&
        std::strcmp(file_extension, format_info.file_extension) == 0) {
      return &format_info;
    }
  }

  return nullptr; /* not found */
}

const format*
format::find_for_file_path(const char* const file_path) {
  assert(file_path != nullptr);

  const char* const file_ending = std::strrchr(file_path, '.');
  if (!file_ending) {
    return nullptr; /* no file extension */
  }

  return find_for_file_extension(file_ending + 1);
}
