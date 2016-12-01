#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# Copyright (C) 2015 Felix Salfelder

AC_PREREQ([2.57])

dnl Read version from file
m4_define([QUCS_VERSION], m4_esyscmd([tr -d '\n' < VERSION]))

AC_INIT([qucs], [QUCS_VERSION], [qucs-bugs@lists.sourceforge.net])
AC_CANONICAL_HOST
# AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Options

# hmm maybe the default should be --disable-doc
AC_ARG_ENABLE([doc],
  AS_HELP_STRING([--disable-doc], [Disable doc subpackage]))
AM_CONDITIONAL([ENABLE_DOC], [test "$enable_doc" != no])

AM_CONDITIONAL([QUCS_TEST], [test -f "$srcdir/qucs-test/run.py"])

dnl Check for MacOSX.
case $host_os in
  *darwin*) MACOSX=yes ;;
  *)        MACOSX=no  ;;
esac
AM_CONDITIONAL(COND_MACOSX, test x$MACOSX = xyes)

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Create Output

AC_CONFIG_SUBDIRS([qucs
                   qucs-core])

AS_IF([test -d "$srcdir/qucs-doc"],
	AC_CONFIG_SUBDIRS([qucs-doc])
)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
