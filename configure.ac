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

dnl Check for libraries:
# libboost (libboost-dev on Ubuntu, boost on Mac OS X + MacPorts)
AX_BOOST_BASE([1.48], [AX_BOOST_UNIT_TEST_FRAMEWORK])
# libraptor2 (libraptor2-dev on Ubuntu, raptor2 on Mac OS X + MacPorts)
AC_CHECK_HEADERS([raptor2/raptor2.h],
  AC_SEARCH_LIBS([raptor_new_world_internal], [raptor2], [],
    AC_MSG_ERROR([*** Shared library libraptor2 not found; install https://github.com/datagraph/raptor ***])),
  AC_MSG_ERROR([*** Header file <raptor2.h> not found; install https://github.com/datagraph/raptor ***]))

dnl Check for header files:

dnl Check for types:

dnl Check for structures:

dnl Check for compiler characteristics:
AC_CANONICAL_HOST
AC_C_BIGENDIAN
AC_C_INLINE
AC_C_RESTRICT

dnl Check for library functions:

dnl Check for system services:

dnl Generate output:
AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile src/rdf++/Makefile test/Makefile test/rdf++/Makefile])
AC_SUBST([PACKAGE_VERSION_MAJOR], ["VERSION_MAJOR"])
AC_SUBST([PACKAGE_VERSION_MINOR], ["VERSION_MINOR"])
AC_SUBST([PACKAGE_VERSION_PATCH], ["VERSION_PATCH"])
AC_CONFIG_FILES([src/rdf++/version.h])
AH_BOTTOM([#include "libcompat.h"])
AC_OUTPUT
