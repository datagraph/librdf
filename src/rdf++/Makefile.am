lib_LTLIBRARIES = librdf++.la

librdf___la_LIBADD = $(top_srcdir)/lib/librfcjson.la

librdf___la_SOURCES =     \
  extern.cc               \
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
  librdf___la_SOURCES += reader/jsonld.cc writer/jsonld.cc
endif

if !DISABLE_NQUADS
  librdf___la_SOURCES += reader/nquads.cc writer/nquads.cc
endif

if !DISABLE_PAJEK
  librdf___la_SOURCES += writer/pajek.cc
endif

if !DISABLE_RDFJSON
  librdf___la_SOURCES += writer/rdfjson.cc
endif

if !DISABLE_TRIX
  librdf___la_SOURCES += reader/trix.cc writer/trix.cc
endif

if !DISABLE_XSLT
  librdf___la_SOURCES += writer/xslt.cc
endif

if HAVE_LIBRAPTOR2
  AM_CPPFLAGS += $(libraptor2_CFLAGS)
  librdf___la_LIBADD += $(libraptor2_LIBS)
  librdf___la_SOURCES += raptor.cc reader/raptor.cc writer/raptor.cc
endif

if HAVE_LIBXML2
  AM_CPPFLAGS += $(libxml2_CFLAGS)
  librdf___la_LIBADD += $(libxml2_LIBS)
endif

if HAVE_LIBXQILLA
endif
