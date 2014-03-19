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
