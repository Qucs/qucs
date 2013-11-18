#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.64)
AC_INIT(qucs-core,0.0.18,qucs-bugs@lists.sourceforge.net)
LT_PREREQ([2.2.2])
LT_INIT([disable-static dlopen win32-dll])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([src/circuit.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([no-define])
AM_MAINTAINER_MODE

AC_PREFIX_DEFAULT([/usr/local])
test "x$prefix" = xNONE && prefix="/usr/local"

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_CHECK_TOOL(AR, ar, :)

dnl Check for parser and lexer generators.
AC_PROG_YACC
AC_PROG_LEX

if test "x$LEX" != "x:"; then
  if test $USE_MAINTAINER_MODE = yes; then
    AC_MSG_CHECKING([for $LEX version])
    [lexver=`eval $LEX --version 2>/dev/null | head -1 | sed 's/[^0-9\.]//g'`]
    if test -n "$lexver"; then
      case "$lexver" in
        [2.5.9 | [2-9].[5-9].[1-9][0-9])]
          AC_MSG_RESULT([$lexver >= 2.5.9])
        ;;
          [*)]
          AC_MSG_ERROR([$lexver < 2.5.9])
        ;;
      esac
    else
      AC_MSG_RESULT([not identified])
    fi
    unset lexver
  fi
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

dnl Check for debugging.
AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug],
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
# disable stripping (debian bug #437876)
#    case $host_os in
#      *darwin*) LDFLAGS="$LDFLAGS" ;;
#      *)        LDFLAGS="$LDFLAGS -s" ;;
#    esac
  fi
fi
unset enable_debug

dnl Yet another compiler check.
if test "x$GCC" = xyes; then
  CFLAGS="$CFLAGS -pipe"
  CXXFLAGS="$CXXFLAGS -pipe -fno-exceptions -fno-rtti -fno-check-new"
  if test $USE_MAINTAINER_MODE = yes; then
    CFLAGS="$CFLAGS -W -Wall -Wmissing-prototypes"
    CXXFLAGS="$CXXFLAGS -W -Wall"
  fi
fi

dnl Check for new platforms.
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

dnl Check for additional programs.
AC_PATH_PROG(GPERF, gperf, :)
if test $USE_MAINTAINER_MODE = yes; then
  if test "$GPERF" = ":"; then
    AC_MSG_ERROR([The $PACKAGE package requires an installed gperf.])
  fi
fi

dnl check for adms
if test $USE_MAINTAINER_MODE = yes; then
  dnl now, to build or not to build the adms subproject. The adms
  dnl subproject is required to generate code during the maintainer
  dnl build process. It is not required for distributions. It is
  dnl only required if MAINTAINER_MODE is true in the Makefile.am's
  dnl
  dnl The default behaviour is "check for adms, if it's not there,
  dnl build and use the bundled copy". Note also that it supports
  dnl --with-mkadms=PATH (user may have a copy installed in an odd
  dnl place), --with-mkadms=internal (needed for distcheck, where
  dnl we want to test everything) and --with-mkadms=external (for
  dnl package maintainers, who don't like bundled subprojects).
  AC_ARG_WITH([mkadms],
    [AS_HELP_STRING([--with-mkadms],
      [Either the full path to adms, or "external", "internal", or "check" @<:@check@:>@])],
    [ADMSXML=$withval],
    [with_mkadms=check])
  AS_IF([test "$with_mkadms" = check -o "$with_mkadms" = external],
    [AC_PATH_PROG([ADMSXML], [mkadms], [no])
     AC_MSG_NOTICE([Using host external admsXml rather than bundled version.])],
    [AC_MSG_NOTICE([Using bundled admsXml.])])
  AS_IF([test "$with_mkadms" = external -a "$ADMSXML" = no],
    [AC_MSG_ERROR([External adms demanded, but not found.])])
  dnl We conditionally set ADMSXML in the top level Makefile.am in
  dnl qucs-core. AM_SUBST_NOTMAKE([ADMSXML]) stops automake from
  dnl generating a line of the form ADMSXML = @ADMSXML@ in Makefile.in.
  dnl We need to do that assignment conditionally.
  AM_SUBST_NOTMAKE([ADMSXML])
else
  dnl not maintainer mode, set with_mkadms to external which will prevent it
  dnl being built
  with_mkadms = external
fi

dnl We set up an automake conditional because we need to do
dnl different stuff for internal/external adms inside Makefile.am
AM_CONDITIONAL([USE_INTERNAL_ADMS],
  [test "$with_mkadms" = internal -o "$ADMSXML" = no])
dnl if we are using internal adms we need to add it to the
dnl subdirectories autoconf will recurse into as it has its
dnl own configure.ac and Makefile.am
AM_COND_IF([USE_INTERNAL_ADMS], [AC_CONFIG_SUBDIRS([adms])])

dnl Checks for libraries.
AC_CHECK_LIB(m, sin)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h ieeefp.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

dnl Check for type sizes.
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
type32="int"
if test $ac_cv_sizeof_short -eq 4; then
  type32=short
elif test $ac_cv_sizeof_int -eq 4; then
  type32=int
elif test $ac_cv_sizeof_long -eq 4; then
  type32=long
fi
AC_DEFINE_UNQUOTED(nr_int32_t, $type32, [C-type for 32-bit integers.])
unset type32
type16="short"
if test $ac_cv_sizeof_short -eq 2; then
  type16=short
elif test $ac_cv_sizeof_int -eq 2; then
  type16=int
elif test $ac_cv_sizeof_long -eq 2; then
  type16=long
fi
AC_DEFINE_UNQUOTED(nr_int16_t, $type16, [C-type for 16-bit integers.])
unset type16

dnl Check for double type.
AC_ARG_ENABLE([double],
  AS_HELP_STRING([--enable-double=TYPE],
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

dnl Checks for library functions.
AC_CHECK_FUNCS([floor pow exp sqrt log10 log cos sin acos asin tan atan sinh \
		cosh tanh fabs modf atan2 jn yn erf erfc round trunc acosh \
		asinh strdup strerror strchr])

dnl Checks for complex classes and functions.
AC_CXX_NAMESPACES
AC_CXX_HAVE_COMPLEX
AS_IF([test "X$ac_cv_cxx_have_complex" = "X"],[AC_MSG_ERROR([Qucs Need a templated complex class])])
AC_CXX_HAVE_TR1_COMPLEX
AC_CHECK_CXX_COMPLEX_FUNCS([cos cosh exp log log10 sin sinh sqrt tan tanh])
AC_CHECK_CXX_COMPLEX_FUNCS([acos acosh asin asinh atan atanh])
AC_CHECK_CXX_COMPLEX_FUNCS([log2 norm])
AC_CHECK_CXX_COMPLEX_POW
AC_CHECK_CXX_COMPLEX_ATAN2
AC_CHECK_CXX_COMPLEX_FMOD
AC_CHECK_CXX_COMPLEX_POLAR
AC_CHECK_CXX_COMPLEX_POLAR_COMPLEX

dnl Definitions at top of <config.h>.
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
                 src/math/Makefile
                 src/interface/Makefile
                 src/converter/Makefile
                 src/components/Makefile
                 src/components/microstrip/Makefile
                 src/components/devices/Makefile
                 src/components/digital/Makefile
                 src/components/verilog/Makefile])
AC_OUTPUT

dnl Print results.
AC_MSG_RESULT([])
AC_MSG_RESULT([ $PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([])
