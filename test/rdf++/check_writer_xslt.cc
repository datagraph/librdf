/* This is free and unencumbered software released into the public domain. */

#define BOOST_TEST_MODULE writer_xslt
#include <boost/test/unit_test.hpp>

#include <rdf++/writer.h> /* for rdf::writer */

#include <rdf++/quad.h>   /* for rdf::quad */
#include <rdf++/term.h>   /* for rdf::term */
#include <rdf++/triple.h> /* for rdf::triple */

#include <cstdio>         /* for stdout */
#include <sstream>        /* for std::ostringstream */

BOOST_AUTO_TEST_CASE(test_ctor) {
  rdf::writer writer(stdout, "application/xml", "UTF-8", "http://example.org/");
  BOOST_CHECK(true); // TODO
}

BOOST_AUTO_TEST_CASE(test_write_triple) {
  const rdf::triple triple = {
    rdf::blank_node("foo"),
    rdf::uri_reference("http://www.w3.org/2000/01/rdf-schema#seeAlso"),
    rdf::blank_node("bar")
  };
  rdf::writer writer(stdout, "application/xml", "UTF-8", "http://example.org/");
  writer.configure("xslt-template", "http://localhost/rdfxml.xsl"); // FIXME
  writer.begin();
  writer.write_triple(triple);
  writer.finish();
  // TODO
}

BOOST_AUTO_TEST_CASE(test_write_quad) {
  const rdf::quad quad = {
    rdf::blank_node("hello"),
    rdf::uri_reference("http://purl.org/dc/terms/title"),
    rdf::plain_literal("Hello, world!", "en-US"),
    rdf::uri_reference("http://example.org/")
  };
  rdf::writer writer(stdout, "application/xml", "UTF-8", "http://example.org/");
  writer.configure("xslt-template", "http://localhost/rdfxml.xsl"); // FIXME
  writer.begin();
  writer.write_quad(quad);
  writer.finish();
  // TODO
}

BOOST_AUTO_TEST_CASE(test_write_quad_without_context) {
  const rdf::quad quad = {
    rdf::blank_node("hello"),
    rdf::uri_reference("http://purl.org/dc/terms/title"),
    rdf::plain_literal("Hello, world!", "en-US"),
  };
  rdf::writer writer(stdout, "application/xml", "UTF-8", "http://example.org/");
  writer.configure("xslt-template", "http://localhost/rdfxml.xsl"); // FIXME
  writer.begin();
  writer.write_quad(quad);
  writer.finish();
  // TODO
}
