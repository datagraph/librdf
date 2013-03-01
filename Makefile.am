AM_CPPFLAGS     = -I$(top_srcdir)/lib -I$(top_srcdir)/src -iquote $(srcdir)
AM_CXXFLAGS     = -Wall -Wextra -pipe
AM_LDFLAGS      = -rdynamic -Xlinker '-z,relro' -Xlinker '--as-needed'
LDADD           = librdf++.la
lib_LTLIBRARIES = librdf++.la

librdf___la_SOURCES =     \
  feature.cc              \
  format.cc               \
  module.cc               \
  quad.cc                 \
  reader.cc               \
  term.cc                 \
  triple.cc               \
  version.cc              \
  writer.cc

base_pkgincludedir = $(includedir)/rdf++

base_pkginclude_HEADERS = \
  feature.h               \
  format.h                \
  module.h                \
  quad.h                  \
  reader.h                \
  term.h                  \
  triple.h                \
  version.h               \
  writer.h
