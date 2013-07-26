/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdf++/quad.h"

#include <cassert> /* for assert() */

using namespace rdf;

quad::quad(term* const subject,
           term* const predicate,
           term* const object,
           term* const context)
  : subject(subject),
    predicate(predicate),
    object(object),
    context(context) {

  assert(subject != nullptr);
  assert(predicate != nullptr);
  assert(object != nullptr);
  /* context can be nullptr to designate the default context */
}

quad::quad(const term* const subject,
           const term* const predicate,
           const term* const object,
           const term* const context)
  : subject(subject != nullptr ? subject->clone() : nullptr),
    predicate(predicate != nullptr ? predicate->clone() : nullptr),
    object(object != nullptr ? object->clone() : nullptr),
    context(context != nullptr ? context->clone() : nullptr) {

  assert(subject != nullptr);
  assert(predicate != nullptr);
  assert(object != nullptr);
  /* context can be nullptr to designate the default context */
}

quad::quad(const term& subject,
           const term& predicate,
           const term& object)
  : subject(subject.clone()),
    predicate(predicate.clone()),
    object(object.clone()),
    context(nullptr) {}

quad::quad(const term& subject,
           const term& predicate,
           const term& object,
           const term& context)
  : subject(subject.clone()),
    predicate(predicate.clone()),
    object(object.clone()),
    context(context.clone()) {}
