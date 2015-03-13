dnl configure.ac                                        -*- Autoconf -*-
dnl
dnl Process this file with `autoconf` to produce a configure script.
dnl
dnl This is free and unencumbered software released into the public domain.
AC_PREREQ([2.68])

dnl Define version information:
m4_define([VERSION_MAJOR],
  m4_esyscmd([cut -d'.' -f1 VERSION | tr -d '\n']))
m4_define([VERSION_MINOR],
  m4_esyscmd([cut -d'.' -f2 VERSION | tr -d '\n']))
m4_define([VERSION_PATCH],
  m4_esyscmd([cut -d'.' -f3 VERSION | tr -d '\n']))
m4_define([VERSION_STRING],
  m4_esyscmd([git describe --dirty --always | tr -d '\n']))

dnl Define package information:
AC_INIT([librdf++], [VERSION_STRING],
  [arto@bendiken.net], [librdf++],
  [https://github.com/datagraph/librdf])

dnl Configure Autoconf:
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_SRCDIR([src/rdf++.h])
AC_CONFIG_AUX_DIR([etc/aclocal])
AC_CONFIG_MACRO_DIR([etc/aclocal])
AC_CONFIG_LIBOBJ_DIR([lib])

dnl Configure Automake:
AM_INIT_AUTOMAKE([foreign -Wall -Werror dist-bzip2 subdir-objects nostdinc])
AM_SILENT_RULES([yes])

dnl Check for programs:
AC_PROG_CC(clang gcc cc)
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_CXX(clang++ g++ c++)
AC_PROG_CXXCPP
AC_USE_SYSTEM_EXTENSIONS
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT
PKG_PROG_PKG_CONFIG([0.24])
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext])

dnl Check for configuration options:
# --with-stdlib=libstdc++|libc++
AC_ARG_WITH([stdlib],
  [AS_HELP_STRING([--with-stdlib=LIB], [specify the C++ standard library to use [default=system]])],
  [], [with_stdlib=system])
AS_IF([test "x$with_stdlib" != "xsystem"],
  [CXXFLAGS="$CXXFLAGS -stdlib=$with_stdlib"
   LDFLAGS="$LDFLAGS -stdlib=$with_stdlib"])
# --with-serd/--without-serd
AC_ARG_WITH([serd],
  [AS_HELP_STRING([--with-serd], [use the Serd library to parse Turtle @<:@default=check@:>@])],
  [], [with_serd=check])
# --enable-debug/--disable-debug
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [build with debugging support [default=no]])],
  [], [enable_debug=no])
AS_IF([test "x$enable_debug" != "xno"],
  [AC_DEFINE([DEBUG], [1], [Enable debugging support.])],
  [AC_DEFINE([NDEBUG], [1], [Disable assertions.])])
AM_CONDITIONAL([ENABLE_DEBUG], [test "x$enable_debug" != "xno"])
# --enable-unicode/--disable-unicode
AC_ARG_ENABLE([unicode],
  [AS_HELP_STRING([--disable-unicode], [omit support for Unicode strings])])
AS_IF([test "x$enable_unicode" == "xno"],
  [AC_DEFINE([DISABLE_UNICODE], 1, [Define to disable Unicode string support.])])
# --enable-*/--disable-*
AC_DEFUN([AX_ARG_ENABLE_FORMAT], [dnl
  m4_define([RDF_FORMAT_ID], m4_translit([$1], [-a-z], [_A-Z]))
  AC_ARG_ENABLE([$1],
    [AS_HELP_STRING([--disable-$1], [omit support for the $2 format])])
  AS_IF([test "x$enable_$1" == "xno"], [
    AC_DEFINE([DISABLE_]RDF_FORMAT_ID, 1, [Define to omit the $2 module.])])
  AM_CONDITIONAL([DISABLE_]RDF_FORMAT_ID, [test "x$enable_$1" == "xno"])
])
AX_ARG_ENABLE_FORMAT([jsonld],  [JSON-LD])  # --enable-jsonld/--disable-jsonld
AX_ARG_ENABLE_FORMAT([nquads],  [N-Quads])  # --enable-nquads/--disable-nquads
AX_ARG_ENABLE_FORMAT([pajek],   [Pajek])    # --enable-pajek/--disable-pajek
AX_ARG_ENABLE_FORMAT([rdfjson], [RDF/JSON]) # --enable-rdfxml/--disable-rdfxml
AX_ARG_ENABLE_FORMAT([rdfxml],  [RDF/XML])  # --enable-rdfxml/--disable-rdfxml
AX_ARG_ENABLE_FORMAT([trig],    [TriG])     # --enable-trig/--disable-trig
AX_ARG_ENABLE_FORMAT([trix],    [TriX])     # --enable-trix/--disable-trix
AX_ARG_ENABLE_FORMAT([turtle],  [Turtle])   # --enable-turtle/--disable-turtle
AX_ARG_ENABLE_FORMAT([xslt],    [XSLT])     # --enable-xslt/--disable-xslt

dnl Check for libraries:
# libserd-0 (libserd-0-0 on Ubuntu, serd on Mac OS X + MacPorts)
AS_IF([test "x$with_serd" != "xno"], [
  PKG_CHECK_MODULES([libserd], [serd-0 >= 0.20],
    [AC_DEFINE([HAVE_SERD], [1], [Define to 1 if you have the libserd shared library.])])
])
AM_CONDITIONAL([HAVE_SERD], [test "x$libserd_LIBS" != "x"])
# libraptor2 (libraptor2-dev on Ubuntu, raptor2 on Mac OS X + MacPorts)
AS_IF([test "x$enable_rdfxml" != "xno" || test "x$enable_trig" != "xno" || test "x$enable_turtle" != "xno" || test "x$enable_xslt" != "xno"], [
  PKG_CHECK_MODULES([libraptor2], [raptor2 >= 2.0.6],
    [AC_DEFINE([HAVE_LIBRAPTOR2], [1], [Define to 1 if you have the libraptor2 shared library.])])
])
AM_CONDITIONAL([HAVE_LIBRAPTOR2], [test "x$libraptor2_LIBS" != "x"])
# libxml2 (libxml2-dev on Ubuntu, libxml2 on Mac OS X + MacPorts)
AS_IF([test "x$enable_trix" != "xno"], [
  PKG_CHECK_MODULES([libxml2], [libxml-2.0 >= 2.7.8],
    [AC_DEFINE([HAVE_LIBXML2], [1], [Define to 1 if you have the libxml2 shared library.])])
])
AM_CONDITIONAL([HAVE_LIBXML2], [test "x$libxml2_LIBS" != "x"])
# libxqilla (libxqilla-dev on Ubuntu, xqilla on Mac OS X + MacPorts)
AS_IF([test "x$enable_xslt" != "xno"], [
  AC_CHECK_HEADERS([xqilla/xqilla-simple.hpp],
    AC_SEARCH_LIBS([createXQillaXQCImplementation], [xqilla],
      [AC_DEFINE([HAVE_LIBXQILLA], [1], [Define to 1 if you have the libxqilla shared library.])],
      AC_MSG_ERROR([*** Shared library libxqilla not found; install http://xqilla.sourceforge.net/ ***])),
    AC_MSG_ERROR([*** Header file <xqilla/xqilla-simple.hpp> not found; install http://xqilla.sourceforge.net/ ***]))
])
AM_CONDITIONAL([HAVE_LIBXQILLA], [test "x$enable_xslt" != "xno" && test "x$ac_res" != "xno"])

dnl Check for header files:

dnl Check for types:

dnl Check for structures:

dnl Check for compiler characteristics:
AC_CANONICAL_HOST
AM_CPPFLAGS="$AM_CPPFLAGS -I\$(top_srcdir)/lib -I\$(top_srcdir)/src -iquote \$(srcdir)"
AM_CXXFLAGS="$AM_CXXFLAGS -Wall -Wextra -pipe"
AM_LDFLAGS="$AM_LDFLAGS"
TEST_CPPFLAGS="$AM_CPPFLAGS -DCATCH_CONFIG_MAIN"
TEST_CXXFLAGS="$AM_CXXFLAGS"
TEST_LDFLAGS="$AM_LDFLAGS"
AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([TEST_CPPFLAGS])
AC_SUBST([TEST_CXXFLAGS])
AC_SUBST([TEST_LDFLAGS])

dnl Check for library functions:
AC_CHECK_FUNCS_ONCE([fmemopen])

dnl Check for system services:

dnl Generate output:
AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile src/rdf++/Makefile test/Makefile])
AC_SUBST([PACKAGE_VERSION_MAJOR], ["VERSION_MAJOR"])
AC_SUBST([PACKAGE_VERSION_MINOR], ["VERSION_MINOR"])
AC_SUBST([PACKAGE_VERSION_PATCH], ["VERSION_PATCH"])
AC_CONFIG_FILES([src/rdf++/version.h])
AH_BOTTOM([#include "libcompat.h"])
AC_OUTPUT
