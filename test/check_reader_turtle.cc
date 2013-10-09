/* This is free and unencumbered software released into the public domain. */

#define BOOST_TEST_MODULE reader_turtle
#include <boost/test/unit_test.hpp>

#include <rdf++/reader.h> /* for rdf::reader */

#include <rdf++/quad.h>   /* for rdf::quad */
#include <rdf++/term.h>   /* for rdf::term */
#include <rdf++/triple.h> /* for rdf::triple */

#include <cstdio>         /* for stdout, fmemopen() */
#include <cstring>

BOOST_AUTO_TEST_CASE(test_ctor) {
  char input_data[] = "[] a <http://www.w3.org/1999/02/22-rdf-syntax-ns#List> .\n";
  FILE* const input = fmemopen(reinterpret_cast<void*>(input_data), sizeof(input_data) - 1, "r");

  BOOST_CHECK_NO_THROW(rdf::reader reader(input, "text/turtle", "UTF-8", "http://example.org/"));
}

BOOST_AUTO_TEST_CASE(test_blank_node_labels) {
  char input_data[] = "[] a <http://www.w3.org/1999/02/22-rdf-syntax-ns#List> .\n";
  FILE* const input = fmemopen(reinterpret_cast<void*>(input_data), sizeof(input_data) - 1, "r");

  rdf::reader reader(input, "text/turtle", "UTF-8", "http://example.org/");
  reader.read_triples([](std::unique_ptr<rdf::triple> triple) {
    BOOST_CHECK(triple != nullptr);
    BOOST_CHECK(triple->subject != nullptr);
    //std::cout << triple->subject->string;
    BOOST_CHECK_NE(triple->subject->string, "genid1");
  });
}
