/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "catch.hpp"

#include <rdf++/term.h> /* for rdf::term */

TEST_CASE("blank_node") {
  rdf::blank_node term("foobar");
  REQUIRE((int)term.type == (int)rdf::term_type::blank_node);
  REQUIRE(term.string == "foobar");
}

TEST_CASE("uri_reference") {
  rdf::uri_reference term("http://example.org/");
  REQUIRE((int)term.type == (int)rdf::term_type::uri_reference);
  REQUIRE(term.string == "http://example.org/");
}

TEST_CASE("plain_literal") {
  rdf::plain_literal term("Hello, world!");
  REQUIRE((int)term.type == (int)rdf::term_type::plain_literal);
  REQUIRE(term.string == "Hello, world!");
}

TEST_CASE("plain_literal_with_language_tag") {
  rdf::plain_literal term("Hello, world!", "en-US");
  REQUIRE((int)term.type == (int)rdf::term_type::plain_literal);
  REQUIRE(term.string == "Hello, world!");
  REQUIRE(term.language_tag == "en-US"); // FIXME
}

TEST_CASE("typed_literal") {
  rdf::typed_literal term("3.1415", "http://www.w3.org/2001/XMLSchema#double");
  REQUIRE((int)term.type == (int)rdf::term_type::typed_literal);
  REQUIRE(term.string == "3.1415");
}

TEST_CASE("dynamic_casting") {
  rdf::plain_literal original("Hello, world!", "en-US");
  const rdf::term* term = &original;
  const rdf::plain_literal* literal = dynamic_cast<const rdf::plain_literal*>(term);
  REQUIRE(literal->language_tag == "en-US");
}
