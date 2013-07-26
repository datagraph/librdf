AM_CPPFLAGS     = -I$(top_srcdir)/lib -I$(top_srcdir)/src -iquote $(srcdir) $(BOOST_CPPFLAGS)
AM_CXXFLAGS     = -Wall -Wextra -pipe
AM_LDFLAGS      = $(BOOST_LDFLAGS)
LDADD           = librdf++.la
lib_LTLIBRARIES = librdf++.la

librdf___la_SOURCES =     \
  extern.cc               \
  feature.cc              \
  format.cc               \
  module.cc               \
  quad.cc                 \
  raptor.cc               \
  reader.cc               \
  reader/jsonld.cc        \
  reader/raptor.cc        \
  reader/trix.cc          \
  term.cc                 \
  triple.cc               \
  version.cc              \
  writer.cc               \
  writer/impl.cc          \
  writer/jsonld.cc        \
  writer/raptor.cc        \
  writer/trix.cc

base_pkgincludedir = $(includedir)/rdf++

base_pkginclude_HEADERS = \
  extern.h                \
  feature.h               \
  format.h                \
  module.h                \
  quad.h                  \
  reader.h                \
  term.h                  \
  triple.h                \
  version.h               \
  writer.h
