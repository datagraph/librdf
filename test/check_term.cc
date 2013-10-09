/* This is free and unencumbered software released into the public domain. */

#define BOOST_TEST_MODULE term
#include <boost/test/unit_test.hpp>

#include <rdf++/term.h> /* for rdf::term */

BOOST_AUTO_TEST_CASE(blank_node) {
  rdf::blank_node term("foobar");
  BOOST_CHECK_EQUAL((int)term.type, (int)rdf::term_type::blank_node);
  BOOST_CHECK_EQUAL(term.string, "foobar");
}

BOOST_AUTO_TEST_CASE(uri_reference) {
  rdf::uri_reference term("http://example.org/");
  BOOST_CHECK_EQUAL((int)term.type, (int)rdf::term_type::uri_reference);
  BOOST_CHECK_EQUAL(term.string, "http://example.org/");
}

BOOST_AUTO_TEST_CASE(plain_literal) {
  rdf::plain_literal term("Hello, world!");
  BOOST_CHECK_EQUAL((int)term.type, (int)rdf::term_type::plain_literal);
  BOOST_CHECK_EQUAL(term.string, "Hello, world!");
}

BOOST_AUTO_TEST_CASE(plain_literal_with_language_tag) {
  rdf::plain_literal term("Hello, world!", "en-US");
  BOOST_CHECK_EQUAL((int)term.type, (int)rdf::term_type::plain_literal);
  BOOST_CHECK_EQUAL(term.string, "Hello, world!");
  BOOST_CHECK_EQUAL(term.language_tag, "en-US"); // FIXME
}

BOOST_AUTO_TEST_CASE(typed_literal) {
  rdf::typed_literal term("3.1415", "http://www.w3.org/2001/XMLSchema#double");
  BOOST_CHECK_EQUAL((int)term.type, (int)rdf::term_type::typed_literal);
  BOOST_CHECK_EQUAL(term.string, "3.1415");
}

BOOST_AUTO_TEST_CASE(dynamic_casting) {
  rdf::plain_literal original("Hello, world!", "en-US");
  const rdf::term* term = &original;
  const rdf::plain_literal* literal = dynamic_cast<const rdf::plain_literal*>(term);
  BOOST_CHECK_EQUAL(literal->language_tag, "en-US");
}
