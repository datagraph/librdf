/* This is free and unencumbered software released into the public domain. */

#include "catch.hpp"

#include <rdf++/reader.h> /* for rdf::reader */

#include <rdf++/quad.h>   /* for rdf::quad */
#include <rdf++/term.h>   /* for rdf::term */
#include <rdf++/triple.h> /* for rdf::triple */

#include <sstream>        /* for std::istringstream */

static const std::string input =
  "_:foo <http://www.w3.org/2000/01/rdf-schema#seeAlso> _:bar .\n"
  "_:hello <http://purl.org/dc/terms/title> \"Hello, world!\"@en-US <http://example.org/> .\n"
  "_:hello <http://purl.org/dc/terms/title> \"Hello, world!\"@en-US .\n";

TEST_CASE("test_ctor") {
  // TODO
}

#if 0 // FIXME
TEST_CASE("test_ctor") {
  std::istringstream stream(input);
  rdf::reader reader(stream, "text/x-nquads", "UTF-8", "http://example.org/");
  // TODO
}
#endif

#if 0 // FIXME
TEST_CASE("test_read_triples") {
  std::istringstream stream(input);
  rdf::reader reader(stream, "text/x-nquads", "UTF-8", "http://example.org/");
  reader.read_triples([](std::unique_ptr<rdf::triple> triple) {
    std::cout << "test_read_triple\n";
  });
  // TODO
}
#endif

#if 0 // FIXME
TEST_CASE("test_read_quads") {
  std::istringstream stream(input);
  rdf::reader reader(stream, "text/x-nquads", "UTF-8", "http://example.org/");
  reader.read_quads([](std::unique_ptr<rdf::quad> quad) {
    std::cout << "test_read_quad\n";
  });
  // TODO
}
#endif
