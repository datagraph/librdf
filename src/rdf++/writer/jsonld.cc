/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "jsonld.h"
#include <rfc/json/json_writer.h>

#include <cassert> /* for assert() */

using namespace rfc4627;

namespace {
  struct implementation : public rdf::writer::implementation {
    implementation(FILE* stream,
      const char* content_type,
      const char* charset,
      const char* base_uri);
    virtual ~implementation() noexcept override;
    virtual void configure(const char* key, const char* value) override;
    virtual void begin() override;
    virtual void finish() override;
    virtual void write_triple(const rdf::triple& triple) override;
    virtual void write_quad(const rdf::quad& quad) override;
    virtual void write_comment(const char* comment) override;
    virtual void flush() override;

  protected:
    void write_id(const rdf::term& term);
    void write_object_term(const rdf::term& term);

  private:
    json_writer _json;
  };
}

rdf::writer::implementation*
rdf_writer_for_jsonld(FILE* const stream,
                      const char* const content_type,
                      const char* const charset,
                      const char* const base_uri) {
  return new implementation(stream, content_type, charset, base_uri);
}

implementation::implementation(FILE* const stream,
                               const char* const content_type,
                               const char* const charset,
                               const char* const base_uri)
  : _json{stream} {
  assert(stream != nullptr);
  (void)content_type, (void)charset, (void)base_uri;
}

implementation::~implementation() noexcept {}

void
implementation::configure(const char* const key,
                          const char* const value) {
  (void)key, (void)value; // TODO
}

void
implementation::begin() {
  _json.begin_array();
}

void
implementation::finish() {
  _json.finish_array();
}

void
implementation::write_triple(const rdf::triple& triple) {
  const auto& subject   = *triple.subject;
  const auto& predicate = *triple.predicate;
  const auto& object    = *triple.object;

  _json.begin_object();
  _json.write_string("@id");
  write_id(subject);
  _json.write_string(predicate.string.c_str());
  write_object_term(object);
  _json.finish_object();

  _count++;
}

void
implementation::write_quad(const rdf::quad& quad) {
  // TODO: graph
  const auto& subject   = *quad.subject;
  const auto& predicate = *quad.predicate;
  const auto& object    = *quad.object;

  _json.begin_object();
  _json.write_string("@id");
  write_id(subject);
  _json.write_string(predicate.string.c_str());
  write_object_term(object);
  _json.finish_object();

  _count++;
}

void
implementation::write_comment(const char* const comment) {
  static_cast<void>(comment); /* JSON doesn't support comments */
}

void
implementation::flush() {
  _json.flush();
}

void
implementation::write_id(const rdf::term& term_) {
  switch (term_.type) {
    case rdf::term_type::uri_reference: {
      const auto& term = dynamic_cast<const rdf::uri_reference&>(term_);
      _json.write_string(term.string.c_str());
      break;
    }
    case rdf::term_type::blank_node: {
      const auto& term = dynamic_cast<const rdf::blank_node&>(term_);
      std::string buffer{"_:"};
      buffer.append(term.string);
      _json.write_string(buffer.c_str());
      break;
    }
    case rdf::term_type::plain_literal:
    case rdf::term_type::typed_literal:
    case rdf::term_type::none:
    default:
      assert(false && "invalid term type for #write_id");
  }
}

void
implementation::write_object_term(const rdf::term& term_) {
  switch (term_.type) {
    case rdf::term_type::uri_reference: {
      const auto& term = dynamic_cast<const rdf::uri_reference&>(term_);
      _json.begin_object();
      _json.write_string("@id");
      _json.write_string(term.string.c_str());
      _json.finish_object();
      break;
    }
    case rdf::term_type::blank_node: {
      const auto& term = dynamic_cast<const rdf::blank_node&>(term_);
      std::string buffer{"_:"};
      buffer.append(term.string);
      _json.begin_object();
      _json.write_string("@id");
      _json.write_string(buffer.c_str());
      _json.finish_object();
      break;
    }
    case rdf::term_type::plain_literal: {
      const auto& term = dynamic_cast<const rdf::plain_literal&>(term_);
      _json.begin_object();
      if (!term.language_tag.empty()) {
        _json.write_string("@language");
        _json.write_string(term.language_tag.c_str());
      }
      _json.write_string("@value");
      _json.write_string(term.string.c_str());
      _json.finish_object();
      break;
    }
    case rdf::term_type::typed_literal: {
      const auto& term = dynamic_cast<const rdf::typed_literal&>(term_);
      _json.begin_object();
      _json.write_string("@type");
      _json.write_string(term.datatype_uri.c_str());
      _json.write_string("@value");
      _json.write_string(term.string.c_str());
      _json.finish_object();
      break;
    }
    case rdf::term_type::none:
    default:
      assert(false && "invalid term type for #write_object_term");
  }
}
