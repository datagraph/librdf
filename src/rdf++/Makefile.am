AM_CPPFLAGS += $(libraptor2_CFLAGS)

lib_LTLIBRARIES = librdf++.la

librdf___la_LIBADD = $(top_srcdir)/lib/librfcjson.la
librdf___la_LIBADD += $(libraptor2_LIBS)

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
  writer.cc

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

if !DISABLE_JSONLD
  librdf___la_SOURCES += writer/jsonld.cc
endif

if !DISABLE_NQUADS
  librdf___la_SOURCES += writer/raptor.cc
endif

if !DISABLE_TRIX
  AM_CPPFLAGS += $(libxml2_CFLAGS)
  librdf___la_LIBADD += $(libxml2_LIBS)
  librdf___la_SOURCES += writer/trix.cc
endif

if !DISABLE_XSLT
  librdf___la_SOURCES += writer/xslt.cc
endif
