#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(qucs,0.0.2,qucs-bugs@lists.sourceforge.net)
AC_CONFIG_SRCDIR([qucs/qucs.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE(1.7.0)
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

# Check for debugging option.
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
  CFLAGS="-O2"
  CXXFLAGS="-O2"
fi
unset enable_debug

# Yet another compiler check.
if test "x$GCC" = xyes; then
  if test $USE_MAINTAINER_MODE = yes; then
    CFLAGS="$CFLAGS -W -Wall -Wmissing-prototypes"
    CXXFLAGS="$CXXFLAGS -W -Wall -Wmissing-prototypes"
  fi
fi

# Release specific.
# AC_CONFIG_SUBDIRS(qucs-core)

# Check for language support.
AC_PATH_PROG(LRELEASE, lrelease, :)
AC_PATH_PROG(LUPDATE, lupdate, :)

# Check for MOC/UIC support.
AC_PATH_PROG(MOC, moc, :)
AC_PATH_PROG(UIC, uic, :)

# Check for X11
AC_PATH_XTRA
X11_INCLUDES="$X_CFLAGS"
X11_LDFLAGS="$X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS"
AC_SUBST(X11_INCLUDES)
AC_SUBST(X11_LDFLAGS)

# Check include path to Qt.
QT_INCLUDES=""
QT_VER=2
AC_MSG_CHECKING([for Qt headers])
paths="$QTDIR/include /usr/local/qt/include /usr/include/qt /usr/include/qt3 \
  /usr/include /usr/X11R6/include/X11/qt /usr/X11R6/include/X11/qt \
  /usr/X11R6/include/qt /usr/X11R6/include/qt2"
for path in $paths; do
  if test -f "$path/qapplication.h"; then
    QT_INCLUDES=$path
    if test -f "$path/qmemarray.h"; then
      QT_VER=3
    fi
    break
  fi
done
if test "x$QT_INCLUDES" != "x"; then
  AC_MSG_RESULT([found in $QT_INCLUDES])
  QT_INCLUDES="-I$QT_INCLUDES"
else
  AC_MSG_ERROR([not found])
fi
AC_SUBST(QT_INCLUDES)

# Check for multi-threaded option.
AC_ARG_ENABLE([mt],
  AC_HELP_STRING([--disable-mt],
                 [link to non-threaded Qt (deprecated)]),
  enable_mt="$enableval",
  [if test $QT_VER = 3; then
     enable_mt="yes"
   else
     enable_mt="no"
   fi])
if test "$enable_mt" = yes; then
  QT_LIB="-lqt-mt"
  QT_INC="-DQT_THREAD_SUPPORT -D_REENTRANT"
  QT_MTS="multi-threaded"
else
  QT_LIB="-lqt"
  QT_INC=""
  QT_MTS="non-threaded"
fi
unset enable_mt

# Check library path to Qt.
QT_LDFLAGS=""
AC_MSG_RESULT([checking for Qt... $QT_VER ($QT_MTS)])
AC_MSG_CHECKING([for Qt library])
paths="$QTDIR/lib /usr/local/qt/lib /usr/lib/qt /usr/lib \
  /usr/X11R6/lib/X11/qt /usr/X11R6/lib/X11/qt /usr/X11R6/lib/qt \
  /usr/X11R6/lib/qt2"
AC_LANG(C++)
for path in $paths; do
  save_LDFLAGS="$LDFLAGS"
  save_CXXFLAGS="$CXXFLAGS"
  LDFLAGS="$LDFLAGS $X11_LDFLAGS -L$path $QT_LIB"
  CXXFLAGS="$CXXFLAGS $X11_INCLUDES $QT_INCLUDES"
  AC_LINK_IFELSE(
    [#include <qapplication.h>
     int main (int argc, char ** argv) {
     QApplication a (argc, argv); a.exec (); return 0; }],
    [QT_LDFLAGS="$path";
     QT_INCLUDES="$QT_INCLUDES $QT_INC";
     break;])
  LDFLAGS="$save_LDFLAGS"
  CXXFLAGS="$save_CXXFLAGS"
done
LDFLAGS="$save_LDFLAGS"
CXXFLAGS="$save_CXXFLAGS"
if test "x$QT_LDFLAGS" != "x"; then
  AC_MSG_RESULT([found in $QT_LDFLAGS])
  QT_LDFLAGS="-L$QT_LDFLAGS $QT_LIB"
else
  AC_MSG_ERROR([not found])
fi
AC_SUBST(QT_LDFLAGS)
AC_LANG(C)

# Create path info.
BITMAP_PATH="$prefix/share/qucs/bitmaps/"
AC_DEFINE_UNQUOTED([BITMAPDIR], ["$BITMAP_PATH"],
  [Where the data files go.])
unset BITMAP_PATH
BIN_PATH="$prefix/bin/"
AC_DEFINE_UNQUOTED([BINARYDIR], ["$BIN_PATH"],
  [Where the binary files go.])
unset BIN_PATH
LANG_PATH="$prefix/share/qucs/lang/"
AC_DEFINE_UNQUOTED([LANGUAGEDIR], ["$LANG_PATH"],
  [Where the language files go.])
unset LANG_PATH
DOC_PATH="$prefix/share/qucs/docs/en/"
AC_DEFINE_UNQUOTED([DOCDIR], ["$DOC_PATH"],
  [Where the documentation files go.])
unset DOC_PATH

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

# Add here all your Makefiles. This are created by configure.
AC_CONFIG_FILES([Makefile 
		 qucs/Makefile
		 qucs/bitmaps/Makefile
		 qucs/components/Makefile
		 qucs/diagrams/Makefile
		 qucs/paintings/Makefile
                 qucs/docs/Makefile
                 qucs/docs/en/Makefile])
AC_OUTPUT

dnl Print results.
AC_MSG_RESULT([])
AC_MSG_RESULT([ $PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([])
