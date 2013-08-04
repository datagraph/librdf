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

TESTS = $(check_PROGRAMS)

AM_DEFAULT_SOURCE_EXT = .cc
