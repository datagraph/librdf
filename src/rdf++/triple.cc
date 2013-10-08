/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/term.h"
#include "rdf++/triple.h"

#include <cassert> /* for assert() */

using namespace rdf;

triple::triple(term* const subject,
               term* const predicate,
               term* const object) noexcept
  : subject(subject),
    predicate(predicate),
    object(object) {

  assert(subject != nullptr);
  assert(predicate != nullptr);
  assert(object != nullptr);
}

triple::triple(const term* const subject,
               const term* const predicate,
               const term* const object)
  : subject(subject ? subject->clone() : nullptr),
    predicate(predicate ? predicate->clone() : nullptr),
    object(object ? object->clone() : nullptr) {}

triple::triple(const term& subject,
               const term& predicate,
               const term& object)
  : subject(subject.clone()),
    predicate(predicate.clone()),
    object(object.clone()) {}

void
triple::reset() noexcept {
  subject.release();
  predicate.release();
  object.release();
}
