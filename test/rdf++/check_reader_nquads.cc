/* This is free and unencumbered software released into the public domain. */

#define BOOST_TEST_MODULE reader_nquads
#include <boost/test/unit_test.hpp>

#include <rdf++/reader.h> /* for rdf::reader */

#include <rdf++/quad.h>   /* for rdf::quad */
#include <rdf++/term.h>   /* for rdf::term */
#include <rdf++/triple.h> /* for rdf::triple */

#include <sstream>        /* for std::istringstream */

static const std::string input =
  "_:foo <http://www.w3.org/2000/01/rdf-schema#seeAlso> _:bar .\n"
  "_:hello <http://purl.org/dc/terms/title> \"Hello, world!\"@en-US <http://example.org/> .\n"
  "_:hello <http://purl.org/dc/terms/title> \"Hello, world!\"@en-US .\n";

BOOST_AUTO_TEST_CASE(test_ctor) {
  BOOST_CHECK(true); // TODO
}

#if 0 // FIXME
BOOST_AUTO_TEST_CASE(test_ctor) {
  std::istringstream stream(input);
  rdf::reader reader(stream, "text/x-nquads", "UTF-8", "http://example.org/");
  BOOST_CHECK(true); // TODO
}
#endif

#if 0 // FIXME
BOOST_AUTO_TEST_CASE(test_read_triples) {
  std::istringstream stream(input);
  rdf::reader reader(stream, "text/x-nquads", "UTF-8", "http://example.org/");
  reader.read_triples([](rdf::triple* triple) {
    std::cout << "test_read_triple\n";
    delete triple;
  });
  BOOST_CHECK(true); // TODO
}
#endif

#if 0 // FIXME
BOOST_AUTO_TEST_CASE(test_read_quads) {
  std::istringstream stream(input);
  rdf::reader reader(stream, "text/x-nquads", "UTF-8", "http://example.org/");
  reader.read_quads([](rdf::quad* quad) {
    std::cout << "test_read_quad\n";
    delete quad;
  });
  BOOST_CHECK(true); // TODO
}
#endif
