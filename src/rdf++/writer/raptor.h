/* This is free and unencumbered software released into the public domain. */

#ifndef RDFXX_WRITER_RAPTOR_H
#define RDFXX_WRITER_RAPTOR_H

#include "rdf++/writer/impl.h"

#include "rdf++/term.h"

#include <raptor2/raptor2.h> /* for raptor_*() */

class rdf::writer::raptor : public rdf::writer::implementation {
  public:
    raptor(
      const std::string& file_path,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    raptor(
      std::ostream& stream,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    raptor(
      FILE* stream,
      const std::string& content_type,
      const std::string& charset,
      const std::string& base_uri);

    virtual ~raptor() override;

    virtual void begin() override;

    virtual void finish() override;

    virtual void flush() override;

    virtual void write_triple(const triple& triple) override;

    virtual void write_quad(const quad& quad) override;

    static void log_callback(void* user_data, raptor_log_message* message);

  private:
    const std::string _content_type;
    const std::string _charset;
    raptor_world* _world = nullptr;
    raptor_uri* _base_uri = nullptr;
    raptor_iostream* _iostream = nullptr;
    raptor_serializer* _serializer = nullptr;
    raptor_statement* _statement = nullptr;

    void initialize(const std::string& base_uri, std::function<raptor_iostream* ()> make_raptor_iostream);

    raptor_term* make_raptor_term(const rdf::term_position pos, const rdf::term& term);

    void write_statement();
};

#endif /* RDFXX_WRITER_RAPTOR_H */
