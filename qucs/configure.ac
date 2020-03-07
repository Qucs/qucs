#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)

dnl Read version from file
m4_define([QUCS_VERSION], m4_esyscmd([tr -d '\n' < VERSION]))

AC_INIT([qucs], [QUCS_VERSION], [qucs-bugs@lists.sourceforge.net], [], [https://sf.net/p/qucs])
AC_CONFIG_SRCDIR([qucs/qucs.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_HEADERS([qt_version])
AC_CONFIG_AUX_DIR([.])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([no-define])
AM_PROG_AR
AC_PREFIX_DEFAULT([/usr/local])
AC_CONFIG_MACRO_DIR([m4])

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_CHECK_TOOL(AR, ar, :)
LT_INIT()

PKG_PROG_PKG_CONFIG

AC_ARG_WITH(qt_name,
    AS_HELP_STRING([--with-qt-name],
        [Use a specific Qt for building. @<:@default=Qt@:>@])
    [ Various platforms provide Qt, Qt4 and/or Qt5 ] )

qt_name=Qt

AS_IF( [ test -n "$with_qt_name" ],
       [ qt_name=$with_qt_name ] )

dnl Check for debugging option.
AC_ARG_ENABLE([debug],
  AC_HELP_STRING([--enable-debug],
                 [include debug output @<:@default=no@:>@]),
  [case "$enableval" in
   yes) enable_qucs_gui_debug="yes" ;;
   no)  enable_qucs_gui_debug="no"  ;;
   *)   enable_qucs_gui_debug="no"  ;;
   esac],
  [enable_qucs_gui_debug="no"])
if test "$enable_qucs_gui_debug" = yes; then
  AC_DEFINE(DEBUG, 1, [Enable debug build.])
  QT_DEF="-DQT_DEBUG"
  CXXFLAGS="$CXXFLAGS -g -O0"
else
  AC_DEFINE(NDEBUG, 1, [Disable debug build.])
  QT_DEF="-DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT"
fi

dnl Check for Linux
case $host_os in
  *linux*) LINUX=yes ;;
  *)       LINUX=no  ;;
esac
AM_CONDITIONAL(COND_LINUX, test x$LINUX = xyes)

dnl Check for Win32.
case $host_os in
  *mingw* | *msys* ) WIN32=yes ;;
  *)       WIN32=no  ;;
esac
AM_CONDITIONAL(COND_WIN32, test x$WIN32 = xyes)

dnl Check for MacOSX.
case $host_os in
  *darwin*) MACOSX=yes ;;
  *)        MACOSX=no  ;;
esac
AM_CONDITIONAL(COND_MACOSX, test x$MACOSX = xyes)

dnl Check for new platforms.
osversion="`grep \"$host\" $srcdir/PLATFORMS 2>/dev/null`"
if test "x$osversion" = "x"; then
  AC_MSG_RESULT([
  The $PACKAGE package has never been compiled or run on this platform.
  Please send an email to <qucs-devel@lists.sourceforge.net> including your
  compiler and Qt version and whether it worked correctly or not.  The
  '$host' line will then be added to the PLATFORMS file included
  in this package.
])
fi
unset osversion

dnl Check for X11
if test x$LINUX == "xyes"; then
  AC_PATH_XTRA
  X11_INCLUDES="$X_CFLAGS"
  X11_LDFLAGS=""
  X11_LIBS="$X_LIBS $X_PRE_LIBS -lX11 $X_EXTRA_LIBS"
  AC_SUBST(X11_INCLUDES)
  AC_SUBST(X11_LDFLAGS)
  AC_SUBST(X11_LIBS)
fi

dnl MacOSX build and runtime environment options
dnl   borrowed from http://cgit.freedesktop.org/libreoffice/core/tree/configure.ac

dnl Default disabled search for SDK
dnl Enabling set the -isysroot, see darwin section below for SDK setup
AC_ARG_ENABLE([sdk],
  AS_HELP_STRING([--enable-sdk],
                 [search for Mac OSX SDK @<:@default=no@:>@. \
                  Disable this option to set custom compiler flags CXX, CXXFLAGS]),
  [case "$enableval" in
   yes) enable_sdk="yes" ;;
   no)  enable_sdk="no"  ;;
   *)   enable_sdk="yes"  ;;
   esac],
  [enable_sdk="no"])

dnl Specify version of SDK
AC_ARG_WITH(macosx-sdk,
    AS_HELP_STRING([--with-macosx-sdk],
        [Use a specific SDK for building.])
    [
                          Usage:     --with-macosx-sdk=<version>

                          e. g.: --with-macosx-sdk=10.6

                          there are 3 options to control the MacOSX build:
                          --with-macosx-sdk (refered as 'sdk' below)
                          --with-macosx-version-min-required (refered as 'min' below)
                          --with-macosx-version-max-allowed (refered as 'max' below)

                          the connection between these value and the default they take is as follow:
                          ( ? means not specified on the command line, s means the SDK version found,
                          constraint: x <= y <= z)

                          ==========================================
                           command line      || config result
                          ==========================================
                          min  | max  | sdk  || min  | max  | sdk  |
                          ?    | ?    | ?    || 10.6 | 10.s | 10.s |
                          ?    | ?    | 10.x || 10.6 | 10.x | 10.x |
                          ?    | 10.x | ?    || 10.6 | 10.s | 10.s |
                          ?    | 10.x | 10.y || 10.6 | 10.x | 10.y |
                          10.x | ?    | ?    || 10.x | 10.s | 10.s |
                          10.x | ?    | 10.y || 10.x | 10.y | 10.y |
                          10.x | 10.y | ?    || 10.x | 10.y | 10.y |
                          10.x | 10.y | 10.z || 10.x | 10.y | 10.z |


                          see: http://developer.apple.com/library/mac/#technotes/tn2064/_index.html
                          for a detailled technical explanation of these variables

                          Note: MACOSX_DEPLOYMENT_TARGET will be set to the value of 'min'.

                          Note that even if in theory using a --with-macosx-version-max-allowed
                          (i.e. the MAC_OS_X_VERSION_MAX_ALLOWED macro) less than the SDK version
                          should work, in practice Apple does not seem to test that, and at least
                          compiling with -DMAC_OS_X_VERSION_MAX_ALLOWED=1060 against the 10.7 SDK
                          fails in a couple of places. Just because of oversights in ifdefs in the SDK
                          headers, but still.
    ],
,)

AC_ARG_WITH(macosx-version-min-required,
    AS_HELP_STRING([--with-macosx-version-min-required],
        [set the minimum OS version needed to run the built Qucs-core])
    [
                          Usage:     --with-macosx-version-min-required=<version>

                          e. g.: --with-macosx-version-min-required=10.6
                          see --with-macosx-sdk for more info
    ],
,)

AC_ARG_WITH(macosx-version-max-allowed,
    AS_HELP_STRING([--with-macosx-version-max-allowed],
        [set the maximum allowed OS version the Qucs-core compilation can use APIs from])
    [
                          Usage:     --with-macosx-version-max-allowed=<version>

                          e. g.: --with-macosx-version-max-allowed=10.6
                          see --with-macosx-sdk for more info
    ],
,)


# use pkg-config, if it exists
if test "x$PKG_CONFIG" != "x"; then

  if ! `$PKG_CONFIG --atleast-version=4.6.0 ${qt_name}Core`; then
     AC_MSG_ERROR([Qt >= 4.6.0 is required.])
  fi

  QT_MAJOR_VERSION=
  AC_MSG_CHECKING([Qt major version])
  AS_IF( [$PKG_CONFIG --atleast-version=5.0.0 ${qt_name}Core ],
         [ QT_MAJOR_VERSION=5 ],
         [ QT_MAJOR_VERSION=4 ] )
  AC_MSG_RESULT( $QT_MAJOR_VERSION )

  # this seems to work for both Qt4 and 5
  RCC_=`$PKG_CONFIG --variable rcc_location ${qt_name}Core`
  UIC_=`$PKG_CONFIG --variable uic_location ${qt_name}Core`
  MOC_=`$PKG_CONFIG --variable moc_location ${qt_name}Core`
  QT_VERSION=`$PKG_CONFIG --modversion ${qt_name}Core`
  AC_SUBST([QT_VERSION])

  # empty for Qt4
  QT_BINPATH=`$PKG_CONFIG --variable host_bins ${qt_name}Core`
fi # if pkg-config exists

  dnl Platform specific, find libraries, setup compiler flags
  case $host_os in
    *linux* | *cygwin* | *mingw* | *msys* | *bsd* )

      dnl Set Clang
      if test "$CXX -dM -E - < /dev/null | grep __clang__" ; then
        CFLAGS="$CFLAGS -pipe"
        CXXFLAGS="$CXXFLAGS -pipe -fno-exceptions -Wno-deprecated-register"
        LDDFLAGS="$LDDFLAGS"
        use_CLANG="yes"
      fi

      dnl Set GCC.
      if test "x$use_CLANG" != "xyes" -a "x$GCC" = "xyes" ; then
        CFLAGS="$CFLAGS -pipe"
        CXXFLAGS="$CXXFLAGS -pipe -fno-exceptions -fno-check-new"
        if test x$WIN32 = xyes; then
          CXXFLAGS="$CXXFLAGS"
        fi
      fi

      if test "$enable_qucs_gui_debug" = yes; then
        CFLAGS="$CFLAGS -W -Wall -Wmissing-prototypes"
        CXXFLAGS="$CXXFLAGS -W -Wall"
      fi

      dnl Enable C++11 support
      CXXFLAGS="$CXXFLAGS -std=c++0x"

# use pkg-config, if it exists
if test "x$PKG_CONFIG" != "x"; then

      PKG_CHECK_MODULES([QTest], [${qt_name}Test])

      PKG_CHECK_MODULES([QT], [${qt_name}Core ${qt_name}Gui ${qt_name}Script ${qt_name}Svg ${qt_name}Xml Qt3Support], [
        QT_CFLAGS="$QT_CFLAGS $QT_DEF -DQT_DEPRECATED_WARNINGS -D_REENTRANT"
        case $host_os in
          *freebsd*) QT_LIBS="$QT_LIBS -pthread" ;;
          *mingw* | *msys*) QT_CFLAGS="$QT_CFLAGS -mthreads -DQT_DLL -DUNICODE" ;;
        esac
      ], [
        AC_MSG_ERROR([pkgconfig did not work, see config.log])
	])
else
  # pkg-config not found
  #
  # Fallback to old logic if pkg-config should fail
  # this could work in principle, hands up, if you want to use it.
  # (in any case it will hide the error above, which is bad)

  dnl handle provided QTDIR environment variable
  dnl   in Slackware when both Qt3 at Qt4 are installed the Qt4 path
  dnl   is in QT4DIR while QTDIR contains the path to Qt3
  QTDIR_lib_paths=""
  if test "x$QT4DIR" != "x"; then
    echo "QT4DIR provided: $QT4DIR"
    QTDIR=$QT4DIR
  elif test "x$QTDIR" != "x"; then
    echo "QTDIR provided: $QTDIR"
    QTDIR_lib_paths="$QTDIR/lib $QTDIR/lib64"
  fi

  QT_CFLAGS="$QT_DEF -DQT3_SUPPORT -DQT_DEPRECATED_WARNINGS -D_REENTRANT"
  [case $host_os in
    *freebsd*)
      QT_LIB="-lQtCore -lQtGui -lQtXml -lQtSvg -lQtScript -pthread" ;;
    *mingw* | *msys*)
      QT_LIB="-lQtCore4 -lQtGui4 -lQtXml4 -lQt3Support4 -lQtSvg4 -lQtScript4"
      QT_CFLAGS="$QT_CFLAGS -mthreads -DQT_DLL -DUNICODE" ;;
  esac]

  dnl Check include path to Qt.
  QT_INCLUDES=""
  AC_MSG_CHECKING([for Qt headers])
  paths="$QTDIR/include $QTDIR/include/qt4 \
    /usr/local/qt4/include /usr/include/qt4 \
    /usr/include /usr/lib/qt/include /usr/X11R6/include/X11/qt4 \
    /usr/X11R6/include/qt4 /usr/X11R6/include /sw/include/qt4 \
    /usr/local/include/qt4"
  for path in $paths; do
    if test -f "$path/Qt/qapplication.h"; then
      QT_INCLUDES=$path
      break
    fi
  done
  if test "x$QT_INCLUDES" != "x"; then
    AC_MSG_RESULT([found in $QT_INCLUDES])
    QT_CFLAGS="$QT_CFLAGS -I$path -I$path/Qt -I$path/QtGui -I$path/QtCore -I$path/QtSvg -I$path/QtXml -I$path/QtScript -I$path/Qt3Support"
  else
    AC_MSG_ERROR([not found])
  fi

  dnl Check library path to Qt.
  QT_LDFLAGS=""
  QT_LIBS=""
  AC_MSG_CHECKING([for Qt4 library])
  paths="$QTDIR_lib_paths /usr/local/qt4/lib /usr/lib/qt4 /usr/lib/qt4/Qtconf /usr/lib \
    /usr/X11R6/lib/X11/qt4 /usr/X11R6/lib/X11/qt4 /usr/X11R6/lib/qt4 \
    /usr/X11R6/lib /sw/lib /usr/lib64/qt4 /usr/X11R6/lib/qt4 /usr/local/lib/qt4"
  AC_LANG(C++)
  for path in $paths; do
    save_LIBS="$LIBS"
    save_LDFLAGS="$LDFLAGS"
    save_CXXFLAGS="$CXXFLAGS"
    LIBS="$LIBS $X11_LIBS $QT_LIB"
    LDFLAGS="$LDFLAGS -L$path"
    CXXFLAGS="$CXXFLAGS $QT_CFLAGS"
    AC_LINK_IFELSE(
        [AC_LANG_SOURCE([#include <Qt/qapplication.h>
                         int main (int argc, char ** argv) {
                         QApplication a (argc, argv); a.exec (); return 0; }])],
                         [
                               QT_LDFLAGS="$path";
                               break;
                         ]
    )
    LIBS="$save_LIBS"
    LDFLAGS="$save_LDFLAGS"
    CXXFLAGS="$save_CXXFLAGS"
  done

  dnl 
  QT_DIR=$QTDIR
  QT_BINPATH="$QTDIR/bin"

  LIBS="$save_LIBS"
  LDFLAGS="$save_LDFLAGS"
  CXXFLAGS="$save_CXXFLAGS"
  # BUG: check for library availibility. use AC_CHECK_LIBS
  # ( no check is better than a broken one! ) (?)
  if test "x$QT_LDFLAGS" != "x"; then
    AC_MSG_RESULT([found in $QT_LDFLAGS])
    QT_LIBS="-L$QT_LDFLAGS $QT_LIB"
  else
    AC_MSG_ERROR([not found])
  fi
fi # end of Qt4 logic when pkg-config is not found


AS_IF([test $QT_MAJOR_VERSION -ge 5],
          [ QT_CFLAGS="$QT_CFLAGS -fPIC"
      PKG_CHECK_MODULES([QtPrintSupport], [${qt_name}PrintSupport])
      PKG_CHECK_MODULES([QtScript], [${qt_name}Script])
	   ])

    QT_CFLAGS="$QT_CFLAGS $QtPrintSupport_CFLAGS $QtScript_CFLAGS"
    QT_LIBS="$QtScript_LIBS $QtPrintSupport_LIBS $QT_LIBS"

    ;;

  *darwin* )

    dnl BUG, with 10.6, the use of -isysroot cause the /Library/Frameworks to be searched
    dnl based on the SDK path. Causing the framework not to be found:
    dnl  ld: framework not found QtGui
    dnl In newer versions (10.7+) the /Library/Frameworks is searched.
    dnl The solution is to manually add symlinks to Qt Frameworks into the 10.6 SDK.
    dnl See also:
    dnl   http://public.kitware.com/Bug/view.php?id=13765
    dnl   http://bugs.python.org/issue14018
    dnl   https://bitbucket.org/Coin3D/coin/wiki/The%20isysroot%20Issue

    dnl Check for Qt in:
    dnl  * Digia default path: /Library/Frameworks
    dnl  * User provided path: QTDIR
    dnl  * Homebrew default:   /usr/local/lib or ${prefix}/Library/Frameworks
    dnl  * MacPorts default:   /opt/local/lib or ${prefix}/Library/Frameworks
    with_qt=no
    AC_MSG_CHECKING([for Qt OS X framework])
    paths="$QTDIR/lib $QTDIR/Library/Frameworks ${prefix}/Library/Frameworks /Library/Frameworks /opt/local/lib /usr/local/lib"
    for path in $paths; do
      if test -f "$path/QtGui.framework/Headers/QApplication"; then
        with_qt=yes
        break
      fi
    done

    dnl Capture dir found for Qt
    QT_DIR="$path"
    QT_BINPATH="$QT_DIR/bin"

    if test "x${with_qt}" = xyes ; then
      AC_MSG_RESULT([found in $path])
      QT_LIBS="-framework QtGui -framework QtCore -framework QtXml -framework QtSvg -framework QtScript -framework QtTest"
      QT_LIBS="$QT_LIBS -framework Qt3Support"
      CPPFLAGS="$CPPFLAGS -I$path/QtCore.framework/Headers"
      CPPFLAGS="$CPPFLAGS -I$path/QtGui.framework/Headers"
      CPPFLAGS="$CPPFLAGS -I$path/QtXml.framework/Headers"
      CPPFLAGS="$CPPFLAGS -I$path/QtSvg.framework/Headers"
      CPPFLAGS="$CPPFLAGS -I$path/QtScript.framework/Headers"
      CPPFLAGS="$CPPFLAGS -I$path/QtTest.framework/Headers"
      CPPFLAGS="$CPPFLAGS -I$path/Qt3Support.framework/Headers"
      QT_LDFLAGS="$QT_LDFLAGS -F$path"
    else
      AC_MSG_ERROR([not found])
    fi

    dnl Define Qt3Support
    QT_DEF="-DQT_SHARED -DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS -DQT_DEPRECATED_WARNINGS"
    CPPFLAGS="$CPPFLAGS $QT_DEF"
    QT_LDFLAGS="$QT_LDFLAGS -headerpad_max_install_names"
    
    dnl Append linker flags
    QT_LIBS="${QT_LIBS} ${QT_LDFLAGS}"

    if test "$enable_qucs_gui_debug" = yes; then
      CPPFLAGS="$CPPFLAGS -W -Wall"
    fi

    dnl Enable C++11 support
    CPPFLAGS="$CPPFLAGS -std=c++0x"

    dnl ===================================================================
    dnl Test if search for SDK was enabled
    AS_IF([test "x$enable_sdk" = "xyes"], [

    dnl Check OS X SDK and compiler
    dnl borrowed from http://cgit.freedesktop.org/libreoffice/core/tree/configure.ac
    AC_MSG_CHECKING([what Mac OS X SDK to use])

    bitness=-m64

    if test -z "$with_macosx_sdk"; then
        if test -d /Developer/SDKs/MacOSX10.6.sdk; then
            with_macosx_sdk=10.6
        elif test -d /Developer-old/SDKs/MacOSX10.6.sdk; then
            with_macosx_sdk=10.6
        elif test -d /Xcode3/SDKs/MacOSX10.6.sdk; then
            with_macosx_sdk=10.6
        elif test -d /Developer/SDKs/MacOSX10.7.sdk; then
            with_macosx_sdk=10.7
        elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk"; then
            with_macosx_sdk=10.6
        elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk"; then
            with_macosx_sdk=10.7
        elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk"; then
            with_macosx_sdk=10.8
        elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk"; then
            with_macosx_sdk=10.9
        elif test -x /usr/bin/xcode-select; then
            xcodepath="`xcode-select -print-path`"
            if test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk"; then
                with_macosx_sdk=10.6
            elif test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk"; then
                with_macosx_sdk=10.7
            elif test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk"; then
                with_macosx_sdk=10.8
            elif test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk"; then
                with_macosx_sdk=10.9
            fi
        fi
        if test -z "$with_macosx_sdk"; then
            AC_MSG_ERROR([Could not figure out the location of a Mac OS X SDK and its version])
        fi
    fi

    case $with_macosx_sdk in
    10.5)
        MACOSX_SDK_VERSION=1050
        ;;
    10.6)
        MACOSX_SDK_VERSION=1060
        ;;
    10.7)
        MACOSX_SDK_VERSION=1070
        ;;
    10.8)
        MACOSX_SDK_VERSION=1080
        ;;
    10.9)
        MACOSX_SDK_VERSION=1090
        ;;
    10.10)
        MACOSX_SDK_VERSION=101000
        ;;
    *)
        AC_MSG_ERROR([with-macosx-sdk $with_macosx_sdk is not a supported value, supported values are 10.5--10])
        ;;
    esac

    # Next find it (again, if we deduced its version above by finding
    # it... but we need to look for it once more in case
    # --with-macosx-sdk was given so that the above search did not
    # happen).
    if test -z "$MACOSX_SDK_PATH"; then
        case $with_macosx_sdk in
        10.5)
            if test -x /usr/bin/xcode-select; then
                xcodepath="`xcode-select -print-path`"
                if test -d "$xcodepath/SDKs/MacOSX$with_macosx_sdk.sdk"; then
                    MACOSX_SDK_PATH="$xcodepath/SDKs/MacOSX$with_macosx_sdk.sdk"
                fi
            elif test -d /Developer/SDKs/MacOSX10.5.sdk; then
                MACOSX_SDK_PATH=/Developer/SDKs/MacOSX10.5.sdk
            elif test -d /Developer-old/SDKs/MacOSX10.5.sdk; then
                MACOSX_SDK_PATH=/Developer-old/SDKs/MacOSX10.5.sdk
            elif test -d /Xcode3/SDKs/MacOSX10.5.sdk; then
                MACOSX_SDK_PATH=/Xcode3/SDKs/MacOSX10.5.sdk
            fi
            ;;
        10.6)
            if test -d /Developer/SDKs/MacOSX10.6.sdk; then
                MACOSX_SDK_PATH=/Developer/SDKs/MacOSX10.6.sdk
            elif test -d /Developer-old/SDKs/MacOSX10.6.sdk; then
                MACOSX_SDK_PATH=/Developer-old/SDKs/MacOSX10.6.sdk
            elif test -d /Xcode3/SDKs/MacOSX10.6.sdk; then
                MACOSX_SDK_PATH=/Xcode3/SDKs/MacOSX10.6.sdk
            elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"; then
                MACOSX_SDK_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk
            elif test -x /usr/bin/xcode-select; then
                xcodepath="`xcode-select -print-path`"
                if test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"; then
                    MACOSX_SDK_PATH="$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"
                fi
            fi
            ;;
        10.7)
            if test -d /Developer/SDKs/MacOSX$with_macosx_sdk.sdk; then
                MACOSX_SDK_PATH=/Developer/SDKs/MacOSX$with_macosx_sdk.sdk
            elif test -d "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"; then
                MACOSX_SDK_PATH=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk
            elif test -x /usr/bin/xcode-select; then
                xcodepath="`xcode-select -print-path`"
                if test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"; then
                    MACOSX_SDK_PATH="$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"
                fi
            fi
            ;;
        10.8|10.9|10.10)
            xcodepath="`xcode-select -print-path`"
            if test -d "$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"; then
                MACOSX_SDK_PATH="$xcodepath/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$with_macosx_sdk.sdk"
            fi
            ;;
        esac
        if test -z "$MACOSX_SDK_PATH"; then
            AC_MSG_ERROR([Could not figure out the location of Mac OS X $with_macosx_sdk SDK])
        fi
    fi
    AC_MSG_RESULT([SDK $with_macosx_sdk at $MACOSX_SDK_PATH])

    if test "$with_macosx_version_min_required" = ""; then
        case $with_macosx_sdk in
        10.5)
            with_macosx_version_min_required="10.5";;
        *)
            with_macosx_version_min_required="10.6";;
        esac
    fi

    if test "$with_macosx_version_max_allowed" = ""; then
        with_macosx_version_max_allowed="$with_macosx_sdk"
    fi

    # export this so that "xcrun" invocations later return matching values
    DEVELOPER_DIR="${MACOSX_SDK_PATH%/SDKs*}"
    DEVELOPER_DIR="${DEVELOPER_DIR%/Platforms*}"
    export DEVELOPER_DIR
    FRAMEWORKSHOME="${prefix}/Library/Frameworks"
    MACOSX_DEPLOYMENT_TARGET="$with_macosx_version_min_required"

    case "$with_macosx_version_min_required" in
    10.5)
        MAC_OS_X_VERSION_MIN_REQUIRED="1050"
        ;;
    10.6)
        MAC_OS_X_VERSION_MIN_REQUIRED="1060"
        ;;
    10.7)
        MAC_OS_X_VERSION_MIN_REQUIRED="1070"
        ;;
    10.8)
        MAC_OS_X_VERSION_MIN_REQUIRED="1080"
        ;;
    10.9)
        MAC_OS_X_VERSION_MIN_REQUIRED="1090"
        ;;
    10.10)
        MAC_OS_X_VERSION_MIN_REQUIRED="101000"
        ;;
    *)
        AC_MSG_ERROR([with-macosx-version-min-required $with_macosx_version_min_required is not a supported value, supported values are 10.5--10])
        ;;
    esac

    if test "$BITNESS_OVERRIDE" = ""; then
        case "$with_macosx_version_min_required" in
        10.5)
            case "$with_macosx_sdk" in
            10.5)
                ;;
            *)
                AC_MSG_WARN([Building with a SDK > 10.5 possibly breaks 10.5 compatibility.])
                add_warning "Building with a SDK > 10.5 possibly breaks 10.5 compatibility."
                ;;
            esac
            ;;
        esac
    fi

    # If no CC and CXX environment vars, try to guess where the compiler is
    #LIBTOOL=libtool
    INSTALL_NAME_TOOL=install_name_tool
    if test -z "$save_CC"; then
        AC_MSG_CHECKING([what compiler to use])
        case $with_macosx_sdk in
        10.5)
            CC="${gccprefix}gcc-4.2 $arch -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
            CXX="${gccprefix}g++-4.2 $arch -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
            INSTALL_NAME_TOOL=`xcrun -find install_name_tool`
            ;;
        10.6)
            # did someone copy her 10.6 sdk into xcode 4 (needed on Mountain Lion)?
            if test "$(echo $MACOSX_SDK_PATH | cut -c1-23)" = "/Applications/Xcode.app"; then
                CC="`xcrun -find gcc` $bitness -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
                CXX="`xcrun -find g++` $bitness -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
            else
                CC="gcc-4.2 $bitness -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
                CXX="g++-4.2 $bitness -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
            fi
            INSTALL_NAME_TOOL=`xcrun -find install_name_tool`
            #LIBTOOL=libtool
            ;;
        10.7|10.8|10.9|10.10)
            if test "$with_macosx_version_min_required" != 10.6; then
                # Use libc++ instead of libstdc++ when possible
                stdlib=-stdlib=libc++
            fi
            #if test "$ENABLE_LTO" = TRUE; then
            #    lto=-flto
            #fi
            CC="`xcrun -find clang` $bitness $lto -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
            CXX="`xcrun -find clang++` $bitness $lto $stdlib -mmacosx-version-min=$with_macosx_version_min_required -isysroot $MACOSX_SDK_PATH"
            INSTALL_NAME_TOOL=`xcrun -find install_name_tool`
            AR=`xcrun -find ar`
            NM=`xcrun -find nm`
            STRIP=`xcrun -find strip`
            #LIBTOOL=`xcrun -find libtool`
            RANLIB=`xcrun -find ranlib`
            ;;
        esac
        AC_MSG_RESULT([$CC and $CXX])
    fi

    case "$with_macosx_version_max_allowed" in
    10.5)
        MAC_OS_X_VERSION_MAX_ALLOWED="1050"
        ;;
    10.6)
        MAC_OS_X_VERSION_MAX_ALLOWED="1060"
        ;;
    10.7)
        MAC_OS_X_VERSION_MAX_ALLOWED="1070"
        ;;
    10.8)
        MAC_OS_X_VERSION_MAX_ALLOWED="1080"
        ;;
    10.9)
        MAC_OS_X_VERSION_MAX_ALLOWED="1090"
        ;;
    10.10)
        MAC_OS_X_VERSION_MAX_ALLOWED="101000"
        ;;
    *)
        AC_MSG_ERROR([with-macosx-version-max-allowed $with_macosx_version_max_allowed is not a supported value, supported values are 10.5--10])
        ;;
    esac

    AC_MSG_CHECKING([that macosx-version-min-required is coherent with macosx-version-max-allowed])
    if test $MAC_OS_X_VERSION_MIN_REQUIRED -gt $MAC_OS_X_VERSION_MAX_ALLOWED; then
        AC_MSG_ERROR([the version minimumn required must be inferior or equal to the version maximum allowed])
    else
        AC_MSG_RESULT([ok])
    fi

    AC_MSG_CHECKING([that macosx-version-max-allowed is coherent with macos-with-sdk])
    if test $MAC_OS_X_VERSION_MAX_ALLOWED -gt $MACOSX_SDK_VERSION; then
        AC_MSG_ERROR([the version maximum allowed cannot be greater thatn the sdk level])
    else
        AC_MSG_RESULT([ok])
    fi
    AC_MSG_NOTICE([MAC_OS_X_VERSION_MIN_REQUIRED=$MAC_OS_X_VERSION_MIN_REQUIRED])
    AC_MSG_NOTICE([MAC_OS_X_VERSION_MAX_ALLOWED=$MAC_OS_X_VERSION_MAX_ALLOWED])

    dnl End test for enable_sdk
    ])

    ;;
esac

dnl Check for language support.
AC_PATH_PROGS(LRELEASE, lrelease-qt4 lrelease, :, [$QT_DIR/bin:$PATH])
AC_PATH_PROGS(LUPDATE, lupdate-qt4 lupdate, :, [$QT_DIR/bin:$PATH])

dnl Check for MOC/UIC/RCC support.
AC_PATH_PROGS(MOC, $MOC_ moc, :, [$QT_BINPATH:$PATH:/])
AC_PATH_PROGS(UIC, $UIC_ uic, :, [$QT_BINPATH:$PATH:/])
AC_PATH_PROGS(RCC, $RCC_ rcc, :, [$QT_BINPATH:$PATH:/])

if test "$MOC" = ":"; then
  AC_MSG_ERROR([Need 'moc' to build, (part of Qt)])
fi

if test "$UIC" = ":"; then
  AC_MSG_ERROR([Need 'uic' to build, (part of Qt)])
fi

if test "$RCC" = ":"; then
  AC_MSG_ERROR([Need 'rcc' to build, (part of Qt)])
fi

dnl check for Qt moveable tabwidgets (introduced in Qt 4.5)
QUCS_CHECK_FUNC_QTABWIDGET_SETMOVABLE

dnl append -O0 to CXXFLAGS if in debug mode and using gcc
dnl to assist with debugging by preventing optimisation.
dnl gcc will use the final invocation of -OX and ignore earlier
dnl values
if test "$enable_qucs_gui_debug" = yes; then
 if test "x$GCC" = xyes; then
    CXXFLAGS="$CXXFLAGS -O0"
    AC_MSG_NOTICE([Appending gcc optimisation flag -O0 due to --enable-debug setting.])
 fi
fi

AC_SUBST(QT_LIBS)
AC_SUBST(QT_CFLAGS)
AC_SUBST(QT_LDFLAGS)
AC_SUBST(QT_MOC)
AC_LANG(C)

dnl Check for additional header files.
AC_CHECK_HEADERS([ieeefp.h])

dnl Check for LibBoard files.
dnl AC_LANG(C++)
dnl AC_CHECK_HEADERS([Board.h])
dnl AC_CHECK_LIB(board, fprintf)
dnl AC_LANG(C)

AC_LANG(C++)
AC_CHECK_LIB([dl], [dlopen])
AC_LANG(C)

dnl Check for path transformation.
case $build_os in
mingw* | *msys* | cygwin*)
  PATHXFORM="cygpath -w"
  ;;
*)
  PATHXFORM="echo"
  ;;
esac
AC_SUBST(PATHXFORM)

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
    contrib/Makefile
    man/Makefile
    examples/Makefile
    qucs-filter/Makefile
    qucs-activefilter/Makefile
    qucs-transcalc/Makefile
    qucs-transcalc/bitmaps/Makefile
    qucs-transcalc/examples/Makefile
    qucs-lib/Makefile
    qucs-lib/library/Makefile
    qucs-attenuator/Makefile
    qucs-attenuator/bitmaps/Makefile
    qucs-rescodes/Makefile
    qucs-powercombining/Makefile
    qucs/Makefile
    qucs/bitmaps/Makefile
    qucs/octave/Makefile
    qucs/components/Makefile
    qucs/diagrams/Makefile
    qucs/paintings/Makefile
    qucs/dialogs/Makefile
    qucs/tests/Makefile
    tests/Makefile
    translations/Makefile
    man/qucs.1
    man/qucsdigi.1
    man/qucsdigilib.1
    man/qucsveri.1
    qucs-activefilter/qucsactivefilter.1
    qucs-attenuator/qucsattenuator.1
    qucs-filter/qucsfilter.1
    qucs-lib/qucslib.1
    qucs-rescodes/qucsrescodes.1
    qucs-transcalc/qucstrans.1
    qucs-powercombining/qucspowercombining.1])

AC_CONFIG_FILES([qucs/qucs], [chmod +x qucs/qucs])

dnl Check for Git short SHA to tag version
dnl The release package should also keep it on the config.h
GIT="unknown"
if test -d "../.git"; then
  m4_define([GIT_REVISION], m4_esyscmd([git log --pretty=format:'%h' -n 1u]))
  AC_DEFINE([GIT], ["GIT_REVISION"], [Git short revision hash.])
  GIT=GIT_REVISION
  echo "\nFound Git clone... $GIT"
fi


AC_OUTPUT

dnl delete the unset enable_qucs_gui_debug variable
unset enable_qucs_gui_debug

dnl Print results.
AC_MSG_RESULT([])
AC_MSG_RESULT([ $PACKAGE version $VERSION ($GIT) configured successfully.])
AC_MSG_RESULT([
Configure Information:
  C++ Compiler    : $CXX
  MOC             : $MOC
  UIC             : $UIC
  RCC             : $RCC
    DEFS          :   $DEFS
    CPPFLAGS      :   $CPPFLAGS
    CXXFLAGS      :   $CXXFLAGS

  Linker          : $LD
    LDFLAGS       :   $LDFLAGS
    LIBS          :   $LIBS

  Prefix          : $prefix
  QTDIR           : $QT_DIR
  QT_CFLAGS       : $QT_CFLAGS
  QT_LIBS         : $QT_LIBS
  QT_DEF          : $QT_DEF
  QT_LDFLAGS      : $QT_LDFLAGS
])
AC_MSG_RESULT([])

dnl vim:ts=8:sw=2:noet
