/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rdfjson.h"
#include <rfc/json/json_writer.h>

#include <cassert> /* for assert() */
#include <map>     /* for std::map */
#include <memory>  /* for std::unique_ptr */
#include <vector>  /* for std::vector */

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
    void write_object_term(const rdf::term& term);

  private:
    using object_set    = std::vector<std::unique_ptr<rdf::term>>;
    using predicate_map = std::map<std::string, object_set>;
    using subject_map   = std::map<std::string, predicate_map>;
    json_writer _json;
    subject_map _data;
  };
}

rdf::writer::implementation*
rdf_writer_for_rdfjson(FILE* const stream,
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
  static_cast<void>(content_type);
  static_cast<void>(charset);
  static_cast<void>(base_uri);
}

implementation::~implementation() noexcept {}

void
implementation::configure(const char* const key,
                          const char* const value) {
  static_cast<void>(key); // TODO
  static_cast<void>(value);
}

void
implementation::begin() {
  /* @see http://www.w3.org/TR/rdf-json/#section-serialization */
  _json.begin_object();
}

void
implementation::finish() {
  /* @see http://www.w3.org/TR/rdf-json/#section-serialization */
  auto& subject_map = _data;
  for (auto& element : subject_map) {
    const auto& subject_string = element.first.c_str();
    auto& predicate_map = element.second;

    _json.write_string(subject_string);
    _json.begin_object();

    for (auto& element : predicate_map) {
      const auto& predicate_uri = element.first.c_str();
      auto& object_set = element.second;

      _json.write_string(predicate_uri);
      _json.begin_array();

      for (const auto& object : object_set) {
        write_object_term(*object);
      }
      object_set.clear();

      _json.finish_array();
    }
    predicate_map.clear();

    _json.finish_object();
  }
  subject_map.clear();

  _json.finish_object();
}

void
implementation::write_triple(const rdf::triple& triple) {
  const auto& subject   = *triple.subject;
  const auto& predicate = *triple.predicate;
  const auto& object    = *triple.object;

  _data[subject.string][predicate.string].emplace_back(object.clone());

  _count++;
}

void
implementation::write_quad(const rdf::quad& quad) {
  const auto& subject   = *quad.subject;
  const auto& predicate = *quad.predicate;
  const auto& object    = *quad.object;

  _data[subject.string][predicate.string].emplace_back(object.clone());

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
implementation::write_object_term(const rdf::term& term_) {
  _json.begin_object();

  switch (term_.type) {
    case rdf::term_type::uri_reference: {
      const auto& term = dynamic_cast<const rdf::uri_reference&>(term_);
      _json.write_string("type");
      _json.write_string("uri");
      _json.write_string("value");
      _json.write_string(term.string.c_str());
      break;
    }

    case rdf::term_type::blank_node: {
      const auto& term = dynamic_cast<const rdf::blank_node&>(term_);
      _json.write_string("type");
      _json.write_string("bnode");
      _json.write_string("value");
      std::string buffer{"_:"};
      buffer.append(term.string);
      _json.write_string(buffer.c_str());
      break;
    }

    case rdf::term_type::plain_literal: {
      const auto& term = dynamic_cast<const rdf::plain_literal&>(term_);
      _json.write_string("type");
      _json.write_string("literal");
      _json.write_string("value");
      _json.write_string(term.string.c_str());
      if (!term.language_tag.empty()) {
        _json.write_string("lang");
        _json.write_string(term.language_tag.c_str());
      }
      break;
    }

    case rdf::term_type::typed_literal: {
      const auto& term = dynamic_cast<const rdf::typed_literal&>(term_);
      _json.write_string("type");
      _json.write_string("literal");
      _json.write_string("value");
      _json.write_string(term.string.c_str());
      _json.write_string("datatype");
      _json.write_string(term.datatype_uri.c_str());
      break;
    }

    case rdf::term_type::none:
    default:
      assert(false && "invalid term type for #write_object_term");
  }

  _json.finish_object();
}
