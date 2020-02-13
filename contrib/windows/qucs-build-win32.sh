#!/bin/bash

#
# This script cross compiles Qucs for Windows 32 using Wine and MinGW-w64.
# Works with OSX and Linux
# The MinGW-w64 is the compiler used by the official Qt 4.8.6 binaries/installer.
# The reason to use it is twofold. We don't need to compile Qt and we can
# easily redistribute the compiler. The same compiler is needed for to build
# and load components in the simulator.
#

## TODO
# * Linux cross compile script

##
# Requires package:     ~/git/qucs/release/qucs-x.x.x.tar.gz
# Extract into:         ~/git/qucs/release/build_win32/qucs-x.x.x
# Setup cross compiler
# Compigure sources
# Compile
# Install binaries into ~/.wine/drive_c/qucs-win32-bin/

## Notes:
# Need to add --disable-dependency-tracking to avoid mixup of host and target header files
# https://software.sandia.gov/trac/acro/ticket/2835

## Setup for Mac OSX host, Win32 target:
#

## 1) Install Wine (using homebrew)
#
# $ brew install wine
# $ brew install --devel --without-x11 wine

## 2) Install Qt into Wine
#
# Download Qt
#   http://download.qt-project.org/official_releases/qt/4.8/4.8.6/qt-opensource-windows-x86-mingw482-4.8.6.exe.mirrorlist
#   latest: qt-opensource-windows-x86-mingw482-4.8.6-1.exe
#
# Install Qt into Wine
# $ wine qt-opensource-windows-x86-mingw482-4.8.6.exe
#
# Follow the installer, note the install path.
# Install into
#   C:\Qt\4.8.6
#
# It expect MinGW 4.8.2 to be installed, we do it next. Leave the MinGW path as:
#   C:\mingw32

## 3) Install MinGW into Wine (MinGW matched to Qt)
#
# Download MinGW 4.8.2
# http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/4.8.2/threads-posix/dwarf/i686-4.8.2-release-posix-dwarf-rt_v3-rev3.7z
#
# Install (extract) MinGW into Wine
# $ 7z x i686-4.8.2-release-posix-dwarf-rt_v3-rev3.7z -o$HOME/.wine/drive_c/
#

## 4) Install Inno Setup 5 (used in `qucs-installer-win32.sh`)
# Install older Inno version due to issues with wine
# Error: * Could not find dependent assembly L"Microsoft.Windows.Common-Controls" (6.0.0.0)
#
# Install this older version to fix the issue:
#  http://files.jrsoftware.org/is/5/isetup-5.5.0.exe	29-May-2012 06:51 	1.8M
# $ wine isetup-5.5.0.exe

# check for tarball as argument
if [ "$#" -ne 1 ]; then
    echo "Provide release tarball as argument..."
    exit
fi

echo "Cross-compiling Qucs for Win32"

# setup Windows Mingw32 and Qt paths for Wine
export WINEPATH="C:\mingw32\bin;C:\Qt\4.8.6\bin"

# Install prefix
WINDIR=${HOME}/.wine/drive_c/qucs-win32-bin

REPO=${PWD}
echo Working from: ${REPO}

mkdir build_win32
cd build_win32

# Process tarball
# Requires package:     ~/git/qucs/release/qucs-x.x.x.tar.gz
TARBALL=$(basename $1)
DIRNAME=$(basename $1 .tar.gz)

echo Using source tarball: $TARBALL
tar xvfz ../$TARBALL
cd $DIRNAME

# Hack #1: create a wrapper for `wine gperf`
# Mixing gperf from host and target (wine, Windows) does now work due to line ending problems, see issue #295.
# Doing GPERF="wine gperf" is not recornized by configure.
# Doing GPERF="~/.wine/mingw/bin/gperf" let it be recognized but the execution will lack the wine prefix.
cat > /tmp/gperf << 'EOF'
#!/bin/sh
wine gperf.exe $@
EOF
chmod u+x /tmp/gperf

# do the same for uic, moc, rcc
cat > /tmp/uic << 'EOF'
#!/bin/sh
wine uic.exe $@
EOF
chmod u+x /tmp/uic

cat > /tmp/moc << 'EOF'
#!/bin/sh
wine moc.exe $@
EOF
chmod u+x /tmp/moc

cat > /tmp/rcc << 'EOF'
#!/bin/sh
wine rcc.exe $@
EOF
chmod u+x /tmp/rcc

# Create environment wrapper for mingw
cat > /tmp/mingw << 'EOF'
# Set QTDIR
export QTDIR=${HOME}/.wine/drive_c/Qt/4.8.6
export AR="wine ar.exe"
export AS="wine as.exe"
export CXX="wine g++.exe"
export CC="wine gcc.exe"
export LD="wine ld.exe"
export DLLTOOL="wine dlltool.exe"
export NM="wine nm.exe"
export OBJDUMP="wine objdump.exe"
export RANLIB="wine ranlib.exe"
export STRIP="wine strip.exe"
export WINDRES="wine windres.exe"
# make sure that the host pkg-config is not found
export PKG_CONFIG=
# point to wrappers
export MOC="/tmp/moc"
export UIC="/tmp/uic"
export RCC="/tmp/rcc"
export GPERF="/tmp/gperf"
exec "$@"
EOF
chmod u+x /tmp/mingw


# Hack #2: Patch Makefile.am to run executables via wine.
# In qucs-core it is needed to run executables (gperfappgen, qucsator) thru wine, on the targed system.
patch -p1 < ${REPO}/contrib/windows/0001-Cross-compile-hack-run-executables-via-wine.patch


# FIXME
#  - no need for triplets with mingw-w64?
#    http://sourceforge.net/apps/trac/mingw-w64/wiki/TypeTriplets
#  - qucs-doc not working yet

if [ -d ${WINDIR} ]
then
	echo
	echo Prefix directory exists: ${WINDIR}
	echo
fi


# Configure and cross-compile
/tmp/mingw ./configure --disable-doc --prefix=${WINDIR}  --target=i386-mingw32 --host=i386-mingw32 --build=i586-linux --program-prefix="" --disable-dependency-tracking
make install


# Use host to build and install qucs-doc
cd qucs-doc
./configure  --prefix=${WINDIR}
make
make install


# Installed files into ~/.wine/drive_c/qucs-win32-bin/
