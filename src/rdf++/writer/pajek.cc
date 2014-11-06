/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pajek.h"

#include <cassert> /* for assert() */
#include <cstdio>  /* for std::fflush(), std::snprintf() */
#include <map>     /* for std::map */
#include <set>     /* for std::set */
#include <utility> /* for std::pair */

namespace {
  class implementation final : public rdf::writer::implementation {
    using edge = std::pair<std::uint64_t, std::uint64_t>;
    FILE* _stream{nullptr};
    std::uint64_t _node{0};
    std::map<std::string, std::uint64_t> _nodes;
    std::set<edge> _edges;

  public:
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
    void write_triple(const rdf::term& subject, const rdf::term& predicate, const rdf::term& object);
    std::uint64_t register_node(const rdf::term& term);
  };
}

rdf::writer::implementation*
rdf_writer_for_pajek(FILE* const stream,
                     const char* const content_type,
                     const char* const charset,
                     const char* const base_uri) {
  return new implementation(stream, content_type, charset, base_uri);
}

implementation::implementation(FILE* const stream,
                               const char* const content_type,
                               const char* const charset,
                               const char* const base_uri)
    : _stream{stream} {
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
implementation::begin() {}

void
implementation::finish() {
  std::fprintf(_stream, "*%s %zu\n", "Vertices", _nodes.size());
  for (const auto& element : _nodes) {
    std::fprintf(_stream, "%lu \"%s\"\n", element.second, element.first.c_str()); // TODO: escaping
  }

  std::fprintf(_stream, "*%s\n", "Arcs");
  for (const auto& element : _edges) {
    std::fprintf(_stream, "%lu %lu\n", element.first, element.second);
  }
}

void
implementation::write_triple(const rdf::triple& triple) {
  const auto& subject   = *triple.subject;
  const auto& predicate = *triple.predicate;
  const auto& object    = *triple.object;
  write_triple(subject, predicate, object);
}

void
implementation::write_quad(const rdf::quad& quad) {
  const auto& subject   = *quad.subject;
  const auto& predicate = *quad.predicate;
  const auto& object    = *quad.object;
  write_triple(subject, predicate, object);
}

void
implementation::write_comment(const char* const comment) {
  static_cast<void>(comment); // TODO
}

void
implementation::flush() {
  std::fflush(_stream);
}

void
implementation::write_triple(const rdf::term& subject,
                             const rdf::term& predicate,
                             const rdf::term& object) {
  _count++;

  if (object.is_literal()) {
    return; /* not supported at present */
  }

  static_cast<void>(predicate); /* not used at present */

  _edges.insert({register_node(subject), register_node(object)});
}

std::uint64_t
implementation::register_node(const rdf::term& term) {
  std::uint64_t id{0};
  std::string label;

  if (term.is_blank_node()) {
    label.reserve(2 + term.string.size());
    label.append("_:");
    label.append(term.string);
  }
  else /*if (term.is_uri_reference())*/ {
    label = term.string;
  }

  if (_nodes.count(label)) {
    id = _nodes[label];
  }
  else {
    id = ++_node;
    _nodes[label] = id;
  }

  return id;
}
