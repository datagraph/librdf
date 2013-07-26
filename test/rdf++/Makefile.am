AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/src -iquote $(srcdir) $(BOOST_CPPFLAGS) -DBOOST_TEST_DYN_LINK
AM_CXXFLAGS = -Wall -Wextra -pipe
AM_LDFLAGS  = $(BOOST_LDFLAGS) $(BOOST_UNIT_TEST_FRAMEWORK_LIB)
LDADD       = $(top_srcdir)/src/rdf++/librdf++.la

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
