#!/bin/bash
#
# This script is used with MSYS2 to create Windows binary packages (win32 and win64).
#
# Notes:
# Install inno setup on host
#   http://www.jrsoftware.org/isinfo.php
#   as of now: isetup-5.5.8.exe
#

# Setup MSYS2
PLATFORMm='unknown'
MINGW='unknown'
if   [[ $MSYSTEM == 'MINGW64' ]]; then
   PLATFORM='win64'
   MINGW='mingw64'
elif [[ $MSYSTEM == 'MINGW32' ]]; then
   PLATFORM='win32'
   MINGW='mingw32'
fi

# Where to find Qt
export QTDIR=/c/msys64/$MINGW

# Where to install the binaries
export PREFIX_QUCS=/c/qucs-$PLATFORM-bin

# Expect tarball in the current directory
echo Build and install Qucs from tarbal...
version="0.0.19"
tar xvfz qucs-${version}.tar.gz
cd qucs-${version}
./configure --disable-doc --prefix=$PREFIX_QUCS --disable-dependency-track
make install
cd ..

# Neet do setup the dependencies for qucs-doc, I (guitorri) have neve done in Windows.
# Build qucs-doc on Linux/Mac, copy directories [tutorial, technical, report] into $PREFIX_QUCS/share/qucs/docs/
# Expect qucs-doc PDF files in the current directory
echo Copy qucs-doc PDFs...
tar xfv qucs-doc-PDF.tar.gz -C $PREFIX_QUCS/share/qucs/docs/

echo Copy shared libraries...
# Copy Qt libraries
export QTDIR=/c/msys64/$MINGW/
cp $QTDIR/bin/Qt3Support4.dll $PREFIX_QUCS/bin/
cp $QTDIR/bin/QtCore4.dll     $PREFIX_QUCS/bin/
cp $QTDIR/bin/QtGui4.dll      $PREFIX_QUCS/bin/
cp $QTDIR/bin/QtNetwork4.dll  $PREFIX_QUCS/bin/
cp $QTDIR/bin/QtXml4.dll      $PREFIX_QUCS/bin/
cp $QTDIR/bin/QtSql4.dll      $PREFIX_QUCS/bin/
cp $QTDIR/bin/QtSvg4.dll      $PREFIX_QUCS/bin/
cp $QTDIR/bin/QtScript4.dll   $PREFIX_QUCS/bin/

# Copy other system libraries
export PREFIX_MSYS=/c/msys64/$MINGW
cp $PREFIX_MSYS/bin/libstdc++-6.dll     $PREFIX_QUCS/bin/
cp $PREFIX_MSYS/bin/libwinpthread-1.dll $PREFIX_QUCS/bin/
cp $PREFIX_MSYS/bin/libpng16-16.dll     $PREFIX_QUCS/bin/
cp $PREFIX_MSYS/bin/zlib1.dll           $PREFIX_QUCS/bin/
# Platform dependet gcc runtime libs
if   [[ $MSYSTEM == 'MINGW64' ]]; then
    cp $PREFIX_MSYS/bin/libgcc_s_seh-1.dll  $PREFIX_QUCS/bin/
elif [[ $MSYSTEM == 'MINGW32' ]]; then
    cp $PREFIX_MSYS/bin/libgcc_s_dw2-1.dll  $PREFIX_QUCS/bin/
fi

# Copy other package contents
# add to installer content GPL notice, Icons, site locations
echo Copy Inno contents...
cp    $HOME/git/qucs/contrib/windows/innosetup/gpl.rtf  $PREFIX_QUCS/
cp -r $HOME/git/qucs/contrib/windows/innosetup/misc     $PREFIX_QUCS/

# Copy locally and update inno script, modify prefix
# Handle UNIX path to WINDOWS path conversion
cp $HOME/git/qucs/contrib/windows/innosetup/qucs.iss .
QUCS_PATH=`cygpath.exe -w $PREFIX_QUCS`
# The bash expansion is eating up the backslash
QUCS_PATH=${QUCS_PATH//\\/\\\\}
sed -b -i "s|define TREE .*|define TREE \"$QUCS_PATH\"|g" qucs.iss

# TODO: version is hardcoded in qucs.iss

# Run Innno Setup, create the package installer
# Having trouble with spaces and backslash. Use PATH instead.
echo Running Inno Setup...
export PATH=/c/Program\ Files\ \(x86\)/Inno\ Setup\ 5/:$PATH
INNO='Compil32.exe'
RUN_INNO="$INNO //cc qucs.iss"
$RUN_INNO

# TODO: change output file name in qucs.iss
echo Binary package in `pwd`/Output/
echo Done!