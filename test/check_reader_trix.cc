/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "catch.hpp"

#include <rdf++/reader.h> /* for rdf::reader */
#include <rdf++/writer.h> /* for rdf::writer */

#include <rdf++/quad.h>   /* for rdf::quad */
#include <rdf++/term.h>   /* for rdf::term */
#include <rdf++/triple.h> /* for rdf::triple */

#include <cstdio>         /* for stdout */

static FILE*
examples() {
  return fopen("examples.xml", "r");
}

TEST_CASE("test_ctor") {
  // TODO
}

TEST_CASE("test_read_triples") {
  rdf::writer writer(stdout, "application/trix", "UTF-8", "http://example.org/");
  rdf::reader reader(examples(), "application/trix", "UTF-8", "http://example.org/");
  writer.begin();
  reader.read_triples([&](std::unique_ptr<rdf::triple> triple) {
    writer.write_triple(*triple);
  });
  writer.finish();
  // TODO
}

TEST_CASE("test_read_quads") {
  rdf::writer writer(stdout, "application/trix", "UTF-8", "http://example.org/");
  rdf::reader reader(examples(), "application/trix", "UTF-8", "http://example.org/");
  writer.begin();
  reader.read_quads([&](std::unique_ptr<rdf::quad> quad) {
    writer.write_quad(*quad);
  });
  writer.finish();
  // TODO
}
