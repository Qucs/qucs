# Original Author was Kalle@kde.org
# I lifted it in some mater. (Stephan Kulow)
# I used much code from Janos Farkas

dnl Process this file with autoconf to produce a configure script.

AC_INIT(acinclude.m4) dnl a source file from your sub dir

dnl This is so we can use kde-common
AC_CONFIG_AUX_DIR(admin)

AC_CANONICAL_SYSTEM 

dnl Automake doc recommends to do this only here. (Janos)
AM_INIT_AUTOMAKE(qucs,0.0.2)
AM_MAINTAINER_MODE

dnl Release specific.
dnl AC_CONFIG_SUBDIRS(qucs-sp)

dnl KDE_SET_PREFIX

AC_PREFIX_DEFAULT(/usr/local)
if test "x$prefix" = "xNONE"; then
  prefix=$ac_default_prefix
  ac_configure_args="$ac_configure_args --prefix $prefix"
fi

dnl generate the config header
AC_CONFIG_HEADERS(config.h) dnl at the distribution this done

dnl Checks for programs.
AC_CHECK_COMPILERS

dnl CXXFLAGS="$NOOPT_CXXFLAGS" dnl __kdevelop[noopt]__
dnl CFLAGS="$NOOPT_CFLAGS" dnl __kdevelop[noopt]__
dnl CXXFLAGS="$CXXFLAGS $USE_EXCEPTIONS" dnl __kdevelop[exc]__

AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)
KDE_PROG_LIBTOOL

all_libraries="$all_libraries $USER_LDFLAGS"
all_includes="$all_includes $USER_INCLUDES"
AC_SUBST(all_includes)
AC_SUBST(all_libraries)

dnl for NLS support. Call them in this order!
dnl WITH_NLS is for the po files
AM_KDE_WITH_NLS

KDE_USE_QT(3)
AC_PATH_QT

dnl KDE_NEED_FLEX dnl __kdevelop__
dnl AC_PROG_YACC dnl __kdevelop__

dnl Perform program name transformation
dnl AC_ARG_PROGRAM


dnl PACKAGE set before

KDE_CREATE_SUBDIRSLIST

dnl Create path info.
BITMAP_PATH="$prefix/share/qucs/bitmaps/"
AC_DEFINE_UNQUOTED([BITMAPDIR], ["$BITMAP_PATH"], [Where the data files go.])
unset BITMAP_PATH
BIN_PATH="$prefix/bin/"
AC_DEFINE_UNQUOTED([BINARYDIR], ["$BIN_PATH"], [Where the binary files go.])
unset BIN_PATH
LANG_PATH="$prefix/share/qucs/lang/"
AC_DEFINE_UNQUOTED([LANGUAGEDIR], ["$LANG_PATH"], [Where the language files go.])
unset LANG_PATH

dnl add here all your Makefiles. This are created by configure
AC_CONFIG_FILES([ Makefile ])
AC_CONFIG_FILES([ qucs/Makefile ])
AC_CONFIG_FILES([ qucs/bitmaps/Makefile ])
AC_CONFIG_FILES([ qucs/sim/Makefile ])
AC_CONFIG_FILES([ qucs/components/Makefile ])
AC_CONFIG_FILES([ qucs/diagrams/Makefile ])
AC_CONFIG_FILES([ qucs/paintings/Makefile ])
AC_OUTPUT

dnl Print results.
AC_MSG_RESULT([])
AC_MSG_RESULT([ $PACKAGE version $VERSION configured successfully.])
AC_MSG_RESULT([])
