/* This is free and unencumbered software released into the public domain. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "serd.h"
#include "../quad.h"
#include "../term.h"
#include "../triple.h"

#include <cassert>     /* for assert() */
#include <cstdarg>     /* for std::vsnprintf() */
#include <cstdio>      /* for std::fprintf(), std::snprintf() */
#include <functional>  /* for std::function */
#include <memory>      /* for std::unique_ptr */
#include <new>         /* for std::bad_alloc */

#include <serd/serd.h> /* for serd_*() */

////////////////////////////////////////////////////////////////////////////////

namespace {
  struct implementation final : public rdf::reader::implementation {
  public:
    implementation(FILE* stream,
      const char* content_type,
      const char* charset,
      const char* base_uri);
    virtual ~implementation() noexcept override;
    virtual void read_triples(std::function<void (std::unique_ptr<rdf::triple>)> callback) override;
    virtual void read_quads(std::function<void (std::unique_ptr<rdf::quad>)> callback) override;
    virtual void abort() override;

  protected:
    using serd_env_ptr = std::unique_ptr<SerdEnv, void(*)(SerdEnv*)>;
    using serd_reader_ptr = std::unique_ptr<SerdReader, void(*)(SerdReader*)>;

    void read();
    static SerdStatus base_sink(void* handle, const SerdNode* uri);
    static SerdStatus prefix_sink(void* handle, const SerdNode* name, const SerdNode* uri);
    static SerdStatus statement_sink(void* handle, SerdStatementFlags flags,
      const SerdNode* graph, const SerdNode* subject, const SerdNode* predicate,
      const SerdNode* object, const SerdNode* object_datatype, const SerdNode* object_lang);
    static SerdStatus end_sink(void* handle, const SerdNode* node);
    static SerdStatus error_sink(void* handle, const SerdError* error);
    static void throw_error(const char* func, SerdStatus status);
    static std::unique_ptr<char> expand_curie_or_uri(const char* type_label,
      const SerdEnv* env, const SerdNode* curie);
    static std::unique_ptr<rdf::term> make_term(const SerdEnv* env,
      const SerdNode* node,
      const SerdNode* node_datatype = nullptr,
      const SerdNode* node_language = nullptr);

  private:
    serd_env_ptr _env{nullptr, serd_env_free};
    serd_reader_ptr _reader{nullptr, serd_reader_free};
    std::function<void (std::unique_ptr<rdf::triple>)> _triple_callback;
    std::function<void (std::unique_ptr<rdf::quad>)> _quad_callback;
  };
}

////////////////////////////////////////////////////////////////////////////////

rdf::reader::implementation*
rdf_reader_for_serd(FILE* const stream,
                    const char* const content_type,
                    const char* const charset,
                    const char* const base_uri) {
  return new implementation(stream, content_type, charset, base_uri);
}

////////////////////////////////////////////////////////////////////////////////

implementation::implementation(FILE* const stream,
                               const char* const content_type,
                               const char* const charset,
                               const char* const base_uri_str) {
  static_cast<void>(content_type); /* not used */
  static_cast<void>(charset);      /* not used */

  assert(stream);

  if (base_uri_str) {
    SerdURI base_uri = SERD_URI_NULL;
    SerdNode base_uri_node = serd_node_new_uri_from_string(
      reinterpret_cast<const std::uint8_t*>(base_uri_str), nullptr, &base_uri);
    _env.reset(serd_env_new(&base_uri_node));
    serd_node_free(&base_uri_node);
  }
  else {
    _env.reset(serd_env_new(nullptr));
  }

  if (!_env) {
    throw std::bad_alloc(); /* out of memory */
  }

  _reader.reset(serd_reader_new(SERD_TURTLE, this, nullptr,
    implementation::base_sink,
    implementation::prefix_sink,
    implementation::statement_sink,
    implementation::end_sink));

  if (!_reader) {
    throw std::bad_alloc(); /* out of memory */
  }

  serd_reader_set_error_sink(_reader.get(), implementation::error_sink, this);
  //serd_reader_set_strict(_reader.get(), true);

  SerdStatus status;
  if ((status = serd_reader_start_stream(_reader.get(),
         stream, reinterpret_cast<const uint8_t*>("(stream)"), false)) != SERD_SUCCESS) {
    throw_error("serd_reader_start_stream", status);
  }
}

implementation::~implementation() noexcept {
  try { abort(); } catch (...) {}
}

void
implementation::read_triples(std::function<void (std::unique_ptr<rdf::triple>)> callback) {
  if (!_reader) return;

  _triple_callback = callback;
  _quad_callback   = nullptr;
  read();
}

void
implementation::read_quads(std::function<void (std::unique_ptr<rdf::quad>)> callback) {
  if (!_reader) return;

  _triple_callback = nullptr;
  _quad_callback   = callback;
  read();
}

void
implementation::abort() {
  if (!_reader) return;

  SerdStatus status;
  if ((status = serd_reader_end_stream(_reader.get())) != SERD_SUCCESS) {
    throw_error("serd_reader_end_stream", status);
  }

  _reader.reset();
}

void
implementation::read() {
  SerdStatus status = SERD_SUCCESS;
  while (status == SERD_SUCCESS) {
    status = serd_reader_read_chunk(_reader.get());
  }
}

////////////////////////////////////////////////////////////////////////////////

SerdStatus
implementation::base_sink(void* const handle,
                          const SerdNode* const uri) {
  if (0) std::fprintf(stderr, "%s: handle=%p uri=<%s>\n", __func__, handle, uri ? uri->buf : nullptr);

  assert(handle);
  const implementation& reader = *reinterpret_cast<implementation*>(handle);

  return serd_env_set_base_uri(reader._env.get(), uri);
}

SerdStatus
implementation::prefix_sink(void* const handle,
                            const SerdNode* const name,
                            const SerdNode* const uri) {
  if (0) std::fprintf(stderr, "%s: handle=%p name='%s' uri=<%s>\n", __func__, handle, name ? name->buf : nullptr, uri ? uri->buf : nullptr);

  assert(handle);
  const implementation& reader = *reinterpret_cast<implementation*>(handle);

  return serd_env_set_prefix(reader._env.get(), name, uri);
}

SerdStatus
implementation::statement_sink(void* const handle,
                               const SerdStatementFlags flags,
                               const SerdNode* const /*graph*/,
                               const SerdNode* const subject,
                               const SerdNode* const predicate,
                               const SerdNode* const object,
                               const SerdNode* const object_datatype,
                               const SerdNode* const object_lang) {
  if (0) std::fprintf(stderr, "%s: handle=%p flags=%d subject=%p predicate=%p object=%p object_datatype=%p object_lang=%p\n", __func__, handle, flags, subject, predicate, object, object_datatype, object_lang);

  assert(handle);
  const implementation& reader = *reinterpret_cast<implementation*>(handle);

  std::unique_ptr<rdf::term> subject_term{make_term(reader._env.get(), subject)};
  std::unique_ptr<rdf::term> predicate_term{make_term(reader._env.get(), predicate)};
  std::unique_ptr<rdf::term> object_term{make_term(reader._env.get(), object, object_datatype, object_lang)};

  if (reader._quad_callback) {
    std::unique_ptr<rdf::quad> quad{new rdf::quad{subject_term.release(), predicate_term.release(), object_term.release()}};
    reader._quad_callback(std::move(quad));
  }
  else if (reader._triple_callback) {
    std::unique_ptr<rdf::triple> triple{new rdf::triple{subject_term.release(), predicate_term.release(), object_term.release()}};
    reader._triple_callback(std::move(triple));
  }

  return SERD_SUCCESS;
}

SerdStatus
implementation::end_sink(void* const handle,
                         const SerdNode* const node) {
  if (0) std::fprintf(stderr, "%s: handle=%p node=%s\n", __func__, handle, node ? node->buf : nullptr);

  return SERD_SUCCESS;
}

SerdStatus
implementation::error_sink(void* const handle,
                           const SerdError* const error) {
  if (0) std::fprintf(stderr, "%s: handle=%p error=%s\n", __func__, handle, error ? error->fmt : nullptr);

  if (!error) {
    throw rdf::reader_error{"failed to parse input"};
  }

  switch (error->status) {
    case SERD_SUCCESS:        return SERD_SUCCESS;
    case SERD_FAILURE:        return SERD_SUCCESS;
    case SERD_ERR_UNKNOWN:    break;
    case SERD_ERR_BAD_SYNTAX: break;
    case SERD_ERR_BAD_ARG:    break;
    case SERD_ERR_NOT_FOUND:  break;
    case SERD_ERR_ID_CLASH:   break;
    case SERD_ERR_BAD_CURIE:  break;
    case SERD_ERR_INTERNAL:   break;
  }

  char error_detail[1024];
  std::vsnprintf(error_detail, sizeof(error_detail), error->fmt, *error->args);

  // TODO: trim "\r\n" from error_detail.

  char error_message[4096];
  std::snprintf(error_message, sizeof(error_message), "%s: %s on line %u, column %u",
    error->filename ? reinterpret_cast<const char*>(error->filename) : "/dev/stdin",
    error_detail, error->line, error->col);

  throw rdf::reader_error{error_message};

  return SERD_SUCCESS; /* never reached */
}

void
implementation::throw_error(const char* const symbol,
                            const SerdStatus status) {
  const char* status_str = nullptr;
  switch (status) {
    case SERD_SUCCESS: /* No error */
      return;
    case SERD_FAILURE: /* Non-fatal failure */
      status_str = "SERD_FAILURE";
      break;
    case SERD_ERR_UNKNOWN: /* Unknown error */
      status_str = "SERD_ERR_UNKNOWN";
      break;
    case SERD_ERR_BAD_SYNTAX: /* Invalid syntax */
      status_str = "SERD_ERR_BAD_SYNTAX";
      break;
    case SERD_ERR_BAD_ARG: /* Invalid argument */
      status_str = "SERD_ERR_BAD_ARG";
      break;
    case SERD_ERR_NOT_FOUND: /* Not found */
      status_str = "SERD_ERR_NOT_FOUND";
      break;
    case SERD_ERR_ID_CLASH: /* Encountered clashing blank node IDs */
      status_str = "SERD_ERR_ID_CLASH";
      break;
    case SERD_ERR_BAD_CURIE: /* Invalid CURIE (e.g. prefix does not exist) */
      status_str = "SERD_ERR_BAD_CURIE";
      break;
    case SERD_ERR_INTERNAL: /* Unexpected internal error (should not happen) */
      status_str = "SERD_ERR_INTERNAL";
      break;
  }
  char error_message[256];
  std::snprintf(error_message, sizeof(error_message), "%s() returned %s", symbol, status_str);
  throw rdf::reader_error{error_message};
}

////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<char>
implementation::expand_curie_or_uri(const char* const type_label,
                                    const SerdEnv* const env,
                                    const SerdNode* const curie_or_uri) {
  SerdNode uri = serd_env_expand_node(env, curie_or_uri);

  if (uri.type == SERD_NOTHING) {
    char error_message[256];
    std::snprintf(error_message, sizeof(error_message),
      "failed to expand the %s node '%s'", type_label, curie_or_uri->buf);
    throw rdf::reader_error{error_message};
  }
  assert(uri.type == SERD_URI);

  return std::unique_ptr<char>{const_cast<char*>(reinterpret_cast<const char*>(uri.buf))};
}

std::unique_ptr<rdf::term>
implementation::make_term(const SerdEnv* const env,
                          const SerdNode* const node,
                          const SerdNode* const node_datatype,
                          const SerdNode* const node_language) {
  std::unique_ptr<rdf::term> term;
  if (node) {
    const char* const term_string = reinterpret_cast<const char*>(node->buf);
    switch (node->type) {
      case SERD_NOTHING: {
        assert(false && "node->type == SERD_NOTHING"); /* should never reach this */
        break;
      }
      case SERD_LITERAL: {
        if (node_datatype) {
          auto term_datatype = expand_curie_or_uri(
            (node_datatype->type == SERD_CURIE) ? "CURIE" : "URI", env, node_datatype);
          term.reset(new rdf::typed_literal{term_string, term_datatype.get()});
        }
        else if (node_language) {
          const char* const term_language = reinterpret_cast<const char*>(node_language->buf);
          term.reset(new rdf::plain_literal{term_string, term_language});
        }
        else {
          term.reset(new rdf::plain_literal{term_string});
        }
        break;
      }
      case SERD_URI: {
        auto uri_string = expand_curie_or_uri("URI", env, node);
        term.reset(new rdf::uri_reference{uri_string.get()});
        break;
      }
      case SERD_CURIE: {
        auto uri_string = expand_curie_or_uri("CURIE", env, node);
        term.reset(new rdf::uri_reference{uri_string.get()});
        break;
      }
      case SERD_BLANK: {
        term.reset(new rdf::blank_node{term_string});
        break;
      }
    }
  }
  return term;
}
