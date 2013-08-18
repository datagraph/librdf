AM_CPPFLAGS += $(TEST_CPPFLAGS)
AM_CXXFLAGS += $(TEST_CXXFLAGS)
AM_LDFLAGS  += $(TEST_LDFLAGS)

LDADD = $(top_srcdir)/src/rdf++/librdf++.la

check_PROGRAMS = \
  check_feature  \
  check_format   \
  check_module   \
  check_quad     \
  check_reader   \
  check_term     \
  check_triple   \
  check_version  \
  check_writer

if !DISABLE_JSONLD
  check_PROGRAMS += check_reader_jsonld check_writer_jsonld
endif

if !DISABLE_NQUADS
  check_PROGRAMS += check_reader_nquads check_writer_nquads
endif

if !DISABLE_RDFXML
  check_PROGRAMS += check_reader_rdfxml check_writer_rdfxml
endif

if !DISABLE_TRIG
  check_PROGRAMS += check_reader_trig check_writer_trig
endif

if !DISABLE_TRIX
  check_PROGRAMS += check_reader_trix check_writer_trix
endif

if !DISABLE_TURTLE
  check_PROGRAMS += check_reader_turtle check_writer_turtle
endif

if !DISABLE_XSLT
  check_PROGRAMS += check_writer_xslt
endif

TESTS = $(check_PROGRAMS)

AM_DEFAULT_SOURCE_EXT = .cc
