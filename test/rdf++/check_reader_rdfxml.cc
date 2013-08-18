/* This is free and unencumbered software released into the public domain. */

#define BOOST_TEST_MODULE reader_rdfxml
#include <boost/test/unit_test.hpp>

#include <rdf++/reader.h> /* for rdf::reader */

#include <rdf++/quad.h>   /* for rdf::quad */
#include <rdf++/term.h>   /* for rdf::term */
#include <rdf++/triple.h> /* for rdf::triple */

#include <cstdio>         /* for stdout */

BOOST_AUTO_TEST_CASE(test_ctor) {
  BOOST_CHECK(true); // TODO
}
