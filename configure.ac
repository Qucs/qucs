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

dnl Debug mode, default: disabled.
AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [Debug mode (disabled by default)]),
  [enable_debug="yes"],
  [enable_debug="no"]
)
         
dnl Show the debug mode decision.
if test "$enable_debug" = yes; then
  AC_MSG_NOTICE([==================================================])
  AC_MSG_NOTICE([Debug mode: Enabled])
  AC_MSG_NOTICE([==================================================])
else
  AC_MSG_NOTICE([==================================================])
  AC_MSG_NOTICE([Debug mode: Disabled])
  AC_MSG_NOTICE([==================================================])
fi

dnl Qt debug mode, default: disabled.
AC_ARG_ENABLE([qt-debug],
  AS_HELP_STRING([--enable-qt-debug], [Qt debug (disabled by default)]),
  [enable_qt_debug="yes"],
  [enable_qt_debug="no"]
)

dnl Show the Qt debug mode decision.
if test "$enable_qt_debug" = yes; then
  AC_MSG_NOTICE([==================================================])
  AC_MSG_NOTICE([Qt debug mode: Enabled])
  AC_MSG_NOTICE([==================================================])
else
  AC_MSG_NOTICE([==================================================])
  AC_MSG_NOTICE([Qt debug mode: Disabled])
  AC_MSG_NOTICE([==================================================])
fi

dnl Documentation build, default: enabled.
AC_ARG_ENABLE([doc],
  AS_HELP_STRING([--disable-doc], [Documentation generation (enabled by default)]),
  [enable_doc="no"],
  [enable_doc="yes"]
)

dnl Show the documentation build decision.
if test "$enable_doc" = yes; then
  AC_MSG_NOTICE([==================================================])
  AC_MSG_NOTICE([Documentation generation: Enabled])
  AC_MSG_NOTICE([==================================================])
else
  AC_MSG_NOTICE([==================================================])
  AC_MSG_NOTICE([Documentation generation: Disabled])
  AC_MSG_NOTICE([==================================================])
fi

AM_CONDITIONAL([ENABLE_DOC], [test "$enable_doc" = "yes"])

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
PYTHON=python
AC_SUBST([PYTHON])

AC_CONFIG_SUBDIRS([qucs
                   qucs-core])

AS_IF([test -d "$srcdir/qucs-doc"],
	AC_CONFIG_SUBDIRS([qucs-doc])
)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
