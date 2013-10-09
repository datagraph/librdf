/* This is free and unencumbered software released into the public domain. */

#define BOOST_TEST_MODULE writer_jsonld
#include <boost/test/unit_test.hpp>

#include <rdf++/writer.h> /* for rdf::writer */

#include <rdf++/quad.h>   /* for rdf::quad */
#include <rdf++/term.h>   /* for rdf::term */
#include <rdf++/triple.h> /* for rdf::triple */

#include <sstream>        /* for std::ostringstream */

BOOST_AUTO_TEST_CASE(test_ctor) {
  //rdf::writer writer(std::cout, "application/ld+json", "UTF-8", "http://example.org/"); // TODO
  BOOST_CHECK(true); // TODO
}
