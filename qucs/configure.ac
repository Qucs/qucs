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
AC_CHECK_PROG(LRELEASE, lrelease, lrelease, :)
AC_CHECK_PROG(LUPDATE, lupdate, lupdate, :)

# Check for MOC/UIC support.
AC_CHECK_PROG(MOC, moc, moc, :)
AC_CHECK_PROG(UIC, uic, uic, :)

# Check include path to Qt.
QINCLUDES=""
AC_MSG_CHECKING([for Qt headers])
paths="$QTDIR/include /usr/local/qt/include /usr/include/qt /usr/include/qt3 /usr/include /usr/X11R6/include/X11/qt /usr/X11R6/include/X11/qt /usr/X11R6/include/qt /usr/X11R6/include/qt2"
for path in $paths; do
  if test -f "$path/qapplication.h"; then
    QINCLUDES=$path
    break
  fi
done
if test "x$QINCLUDES" != "x"; then
  AC_MSG_RESULT([found in $QINCLUDES])
  QINCLUDES="-I$QINCLUDES"
else
  AC_MSG_ERROR([not found])
fi
AC_SUBST(QINCLUDES)

# Check library path to Qt.
QLDFLAGS=""
AC_MSG_CHECKING([for Qt library])
paths="$QTDIR/lib /usr/local/qt/lib /usr/lib/qt /usr/lib /usr/X11R6/lib/X11/qt /usr/X11R6/lib/X11/qt /usr/X11R6/lib/qt /usr/X11R6/lib/qt2"
AC_LANG(C++)
for path in $paths; do
  save_LDFLAGS="$LDFLAGS"
  save_CXXFLAGS="$CXXFLAGS"
  LDFLAGS="$LDFLAGS -L$path -lqt-mt"
  CXXFLAGS="$CXXFLAGS $QINCLUDES"
  AC_LINK_IFELSE(
    [#include <qapplication.h>
     int main (int argc, char ** argv) {
     QApplication a (argc, argv); a.exec (); return 0; }],
    [QLDFLAGS="$path"; QINCLUDES="$QINCLUDES -D_REENTRANT"; break;])
  LDFLAGS="$save_LDFLAGS"
  CXXFLAGS="$save_CXXFLAGS"
done
LDFLAGS="$save_LDFLAGS"
CXXFLAGS="$save_CXXFLAGS"
if test "x$QLDFLAGS" != "x"; then
  AC_MSG_RESULT([found in $QLDFLAGS])
  QLDFLAGS="-L$QLDFLAGS -lqt-mt"
else
  AC_MSG_ERROR([not found])
fi
AC_SUBST(QLDFLAGS)
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

# Add here all your Makefiles. This are created by configure.
AC_CONFIG_FILES([Makefile 
		 qucs/Makefile
		 qucs/bitmaps/Makefile
		 qucs/sim/Makefile
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
