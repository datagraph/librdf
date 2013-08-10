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
  writer/raptor.cc

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

if !DISABLE_TRIX
  librdf___la_SOURCES += writer/trix.cc
endif
