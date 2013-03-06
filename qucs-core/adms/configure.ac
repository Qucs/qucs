#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([adms],[2.3.0],[r29173@gmail.com])
AC_CONFIG_SRCDIR([admsXml.c])
AC_CONFIG_HEADERS([config.h])

# specific configuration.
AC_CONFIG_AUX_DIR([auxconf])
AM_MAINTAINER_MODE

dnl disabling cache loading -->> make easier cross-platform debug
dnl ref: info autoconf, look for AC_CACHE_LOAD
define([AC_CACHE_LOAD], )dnl
define([AC_CACHE_SAVE], )dnl

# Automake Initialisation.
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_YACC
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S

#force use of flex/bison
if test "x$LEX" != "xflex"; then
  AC_MSG_WARN(Program flex not found (found $LEX))
  AC_MSG_ERROR(Please install gnu flex from http://www.gnu.org/software/flex/)
fi
if test "x$YACC" != "xbison -y"; then
  AC_MSG_WARN(Program bison not found (found $YACC))
  AC_MSG_ERROR(Please install gnu bison from http://www.gnu.org/software/bison/)
fi

#advice use of gcc
if test "x$GCC" = "xyes"; then
  case "$CFLAGS" in
  *-Wall*)
    # already present
    ;;
  *)
    CFLAGS="$CFLAGS -Wall"
  esac
else
  AC_MSG_WARN(Seems that the selected C-compiler is not gnu gcc C-compiler)
  AC_MSG_WARN(We advice you to use gcc as C-compiler)
  AC_MSG_WARN(You can install it from http://www.gnu.org/software/gcc/)
fi

# Libtool initialisation.
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])
LT_INIT

dnl perl and libXML
AC_PATH_PROG(PERL, perl, :)
if test $USE_MAINTAINER_MODE = yes; then
  if test "$PERL" = ":"; then
    AC_MSG_ERROR([The $PACKAGE package requires an installed perl.])
  else
    AC_MSG_CHECKING(for XML::LibXML Perl module)
    have_xml="`$PERL -MXML::LibXML -e 'exit 0;' >/dev/null 2>&1`"
    if test $? != "0"; then
      AC_MSG_RESULT(failed)
      AC_MSG_ERROR([Perl package XML::LibXML may be downloaded from http://search.cpan.org/dist/libXML])
    else
      AC_MSG_RESULT(ok)
    fi
    AC_MSG_CHECKING(for GD Perl module)
    have_gd="`$PERL -MGD -e 'exit 0;' >/dev/null 2>&1`"
    if test $? != "0"; then
      AC_MSG_RESULT(failed)
      AC_MSG_ERROR([Perl package GD:: may be downloaded from http://search.cpan.org/dist/GD])
    else
      AC_MSG_RESULT(ok)
    fi
  fi
fi

# Checks for libraries.
AC_CHECK_LIB([m], [pow])

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([float.h inttypes.h libintl.h locale.h malloc.h stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor memset pow putenv setenv sqrt strdup strstr strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
