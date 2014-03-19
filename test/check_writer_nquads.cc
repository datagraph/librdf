/* This is free and unencumbered software released into the public domain. */

#include "catch.hpp"

#include <rdf++/writer.h> /* for rdf::writer */

#include <rdf++/quad.h>   /* for rdf::quad */
#include <rdf++/term.h>   /* for rdf::term */
#include <rdf++/triple.h> /* for rdf::triple */

#include <sstream>        /* for std::ostringstream */

TEST_CASE("test_ctor") {
  rdf::writer writer(std::cout, "text/x-nquads", "UTF-8", "http://example.org/");
  // TODO
}

TEST_CASE("test_define_prefix") {
  // TODO
}

#if 0 // FIXME
TEST_CASE("test_write_triple") {
  std::ostringstream stream;
  const rdf::triple triple = {
    rdf::blank_node("foo"),
    rdf::uri_reference("http://www.w3.org/2000/01/rdf-schema#seeAlso"),
    rdf::blank_node("bar")
  };
  rdf::writer writer(stream, "text/x-nquads", "UTF-8", "http://example.org/");
  writer.begin();
  writer.write_triple(triple);
  writer.finish();
  REQUIRE(stream.str() == "_:foo <http://www.w3.org/2000/01/rdf-schema#seeAlso> _:bar .\n");
}
#endif

#if 0 // FIXME
TEST_CASE("test_write_quad") {
  std::ostringstream stream;
  const rdf::quad quad = {
    rdf::blank_node("hello"),
    rdf::uri_reference("http://purl.org/dc/terms/title"),
    rdf::plain_literal("Hello, world!", "en-US"),
    rdf::uri_reference("http://example.org/")
  };
  rdf::writer writer(stream, "text/x-nquads", "UTF-8", "http://example.org/");
  writer.begin();
  writer.write_quad(quad);
  writer.finish();
  REQUIRE(stream.str() == "_:hello <http://purl.org/dc/terms/title> \"Hello, world!\"@en-US <http://example.org/> .\n");
}
#endif

#if 0 // FIXME
TEST_CASE("test_write_quad_without_context") {
  std::ostringstream stream;
  const rdf::quad quad = {
    rdf::blank_node("hello"),
    rdf::uri_reference("http://purl.org/dc/terms/title"),
    rdf::plain_literal("Hello, world!", "en-US"),
  };
  rdf::writer writer(stream, "text/x-nquads", "UTF-8", "http://example.org/");
  writer.begin();
  writer.write_quad(quad);
  writer.finish();
  REQUIRE(stream.str() == "_:hello <http://purl.org/dc/terms/title> \"Hello, world!\"@en-US .\n");
}
#endif
