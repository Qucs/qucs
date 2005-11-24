#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(qucs-core,0.0.8,qucs-bugs@lists.sourceforge.net)
AC_CONFIG_SRCDIR([src/circuit.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([no-define])
AM_MAINTAINER_MODE
AC_PREFIX_DEFAULT([/usr/local])
test "x$prefix" = xNONE && prefix="/usr/local"

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Check for parser and lexer generators.
AC_PROG_YACC
AC_PROG_LEX

if test "x$LEX" != "x:"; then
  AC_MSG_CHECKING([whether $LEX accepts --nounistd])
  cat <<EOF > lextest.l
%%
token     action;
EOF
  lexout="`$LEX --nounistd -olextest.c lextest.l 2>&1`"
  rm -f lextest.l lextest.c
  if test "x$lexout" != "x"; then
    AC_MSG_RESULT([no])
  else
    AC_MSG_RESULT([yes])
    LEX="$LEX --nounistd"
  fi
  unset lexout
fi

# Check for debugging.
AC_ARG_ENABLE([debug],
  AC_HELP_STRING([--enable-debug],
                 [include debug output @<:@default=no@:>@]),
  [case "$enableval" in
   yes) enable_debug="yes" ;;
   no)  enable_debug="no"  ;;
   *)   enable_debug="no"  ;;
   esac],
  [enable_debug="no"])
if test $USE_MAINTAINER_MODE = yes; then
  enable_debug="yes"
fi
if test "$enable_debug" = yes; then
  AC_DEFINE(DEBUG, 1, [Define if debug output should be supported.])
else
  AC_DEFINE(NDEBUG, 1, [Define if debug code should be suppressed.])
  if test "x$GCC" = xyes; then
    CFLAGS="`echo $CFLAGS | sed -e 's/\-g //g'`"
    CXXFLAGS="`echo $CXXFLAGS | sed -e 's/\-g //g'`"
  fi
fi
unset enable_debug

# Yet another compiler check.
if test "x$GCC" = xyes; then
  CFLAGS="$CFLAGS -pipe"
  CXXFLAGS="$CXXFLAGS -pipe -fno-exceptions -fno-rtti -fno-check-new"
  if test $USE_MAINTAINER_MODE = yes; then
    CFLAGS="$CFLAGS -W -Wall -Wmissing-prototypes"
    CXXFLAGS="$CXXFLAGS -W -Wall"
  fi
fi

# Check for new platforms.
osversion="`grep \"$host\" $srcdir/doc/PLATFORMS 2>/dev/null`"
if test "x$osversion" = "x"; then
  AC_MSG_RESULT([
  The $PACKAGE package has never been compiled or run on this platform.  
  Please send an email to <qucs-devel@lists.sourceforge.net> including your
  compiler version and whether it worked correctly or not.  '$host' 
  will then be added to the PLATFORMS file included in this package.
])
fi
unset osversion

# Checks for libraries.
AC_CHECK_LIB(m, sin)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h ieeefp.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Check for double type.
AC_ARG_ENABLE([double],
  AC_HELP_STRING([--enable-double=TYPE],
                 [type of double representation @<:@default=double@:>@]),
  [case "$enableval" in
   no | yes | double)
     double_type="double"
     double_size=8
     ;;
   ldouble)
     double_type="long double"
     AC_CHECK_SIZEOF([long double],12)
     double_size=$ac_cv_sizeof_long_double
     ;;
   float)
     double_type="float"
     double_size=4
     ;;
   *)
     double_type="none"
     double_size=0
     ;;
   esac],
  [double_type="double"; double_size=8])
if test "$double_type" = none; then
  AC_MSG_ERROR([valid types are double, float and ldouble.])
fi
AC_DEFINE_UNQUOTED(nr_double_t, $double_type,
  [The global type of double representation.])
AC_DEFINE_UNQUOTED(NR_DOUBLE_SIZE, $double_size,
  [The size of the double representation.])
unset double_type
unset double_size

# Checks for library functions.
AC_CHECK_FUNCS([floor pow exp sqrt log10 log cos sin tan atan sinh cosh tanh \
                fabs modf atan2 strdup strerror strchr])

# Definitions at top of <config.h>.
AH_TOP([
/* __BEGIN_DECLS should be used at the beginning of your declarations,
   so that C++ compilers don't mangle their names.  Use __END_DECLS at
   the end of C declarations. */
#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
])

dnl Add here all your Makefiles. These are created by configure.
AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 src/Makefile
                 src/converter/Makefile
                 src/components/Makefile
                 src/components/microstrip/Makefile
                 src/components/devices/Makefile
                 src/components/digital/Makefile])
AC_OUTPUT

# Print results.
AC_MSG_RESULT([])
AC_MSG_RESULT([ $PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([])
