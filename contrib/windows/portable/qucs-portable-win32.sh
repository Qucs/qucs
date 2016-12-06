#!/bin/bash
#
# This script builds Windows 32 portable package.
#
# Based on https://github.com/Qucs/qucs/wiki/Build-Windows
# Section about - MinGW 4.8.2 - Qt 4.8.6 - MSYS shell
# Reasons to do that:
#   - Use prebuild official Qt libs
#   - Use and redistribute the available MinGW package (matched to Qt)
#
# Notes:
# - bootstrap will fail, due to blacklisted automake in MSYS
# - need boostraped tarball made elsewhere with `make dist`
#
# TODO:
# - add contrib/ to `make dist`
# - craete target like  `make portable`

# run this script from the root of the working tree, ex. ~/git/qucs/

REPO=${PWD}
echo Working from: ${REPO}

# check for tarball as argument
if [ "$#" -ne 1 ]; then
    echo "Provide release tarball as argument..."
    exit
fi

# Assume tarball at  ~/git/qucs/qucs-x.x.x.tar.gz
TARBALL=$(basename $1)
DIRNAME=$(basename $1 .tar.gz)

echo Using source tarball: $TARBALL
tar xvfz $TARBALL
cd $DIRNAME


# install prefix
WINDIR=/c/qucs-win32-bin

# Set QTDIR
export QTDIR=/C/Qt/4.8.6


./configure --disable-doc --prefix=${WINDIR} --disable-dependency-tracking
make install


# add to installer content GPL notice, Icons, site locations
cp ${REPO}/qucs/COPYING $WINDIR
cp ${REPO}/NEWS.md $WINDIR
cp ${REPO}/contrib/windows/portable/README.md $WINDIR
cp ${REPO}/contrib/windows/portable/qucs.bat $WINDIR

echo Copy runtime libraries...
cp $QTDIR/bin/Qt3Support4.dll $WINDIR/bin
cp $QTDIR/bin/QtCore4.dll     $WINDIR/bin
cp $QTDIR/bin/QtGui4.dll      $WINDIR/bin
cp $QTDIR/bin/QtNetwork4.dll  $WINDIR/bin
cp $QTDIR/bin/QtXml4.dll      $WINDIR/bin
cp $QTDIR/bin/QtSql4.dll      $WINDIR/bin
cp $QTDIR/bin/QtSvg4.dll      $WINDIR/bin
cp $QTDIR/bin/QtScript4.dll   $WINDIR/bin

cp $QTDIR/bin/libgcc_s_dw2-1.dll  $WINDIR/bin
cp $QTDIR/bin/libstdc++-6.dll     $WINDIR/bin
cp $QTDIR/bin/libwinpthread-1.dll $WINDIR/bin


# TODO
# - copy qucs-doc pdfs
#
# TODO (optional)
# - bundle asco
# - bundle admsXml,
# - bundle freehdl
#
# Update Readme info about
# - mingw
# - octave
# - icarus

# TODO create redistributable zip at this point


