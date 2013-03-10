/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/term.h"

using namespace rdf;

term::term(const term_type type, const std::string& value)
  : type(type), value(value) {}

term::~term() = default;
