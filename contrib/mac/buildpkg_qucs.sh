#!/bin/sh
#
# Build a simple flat package for Qucs
#
# 0) extract the release tarball
# 1) select the target configure for Qucs
# 2) Make
# 3) Install to DESTDIR, eg. `make instal DESTDIR=/tmp/installdir`
# 4) This script will create a package out of the 'DESTDIR' directory
#
#
# NOTES
# - Only Intel 64 architecture is supported.
# - The OSX SDK version is now handled by the configure.ac
#   - it sets -stdlib=libstdc++ for OS X 10.6
#   - it sets -stdlib=libc++     for OS X 10.7 and higher

## References:
# http://vincent.bernat.im/en/blog/2013-autoconf-osx-packaging.html
# http://crushbeercrushcode.org/2014/01/using-pkgbuild-and-productbuild-on-os-x-10-7/
# http://stackoverflow.com/questions/11487596/making-os-x-installer-packages-like-a-pro-xcode4-developer-id-mountain-lion-re

## About uninstaller:
# http://superuser.com/questions/36567/how-do-i-uninstall-any-apple-pkg-package-file

# TODO
# - pass tarball location and paramters to this script
# - change the package in such a way the user password is not needed.
#


# Package stamp
version="0.0.19"
#date=`date "+%Y%m%d"`

tar xvfz qucs-${version}.tar.gz
cd qucs-${version}


## configure one of the following targets

VER=7 # 10.7+
#VER=6 # 10.6
#VER=5 # 10.5 #issues detecting this SDK

if [ $VER -eq 7 ]; then
  echo "Configure for OSX 10.7 to 10.11 (libc++)"

./configure --prefix=/usr/local --disable-adms \
--with-macosx-sdk=10.9 \
--with-macosx-version-min-required=10.7 \
--with-macosx-version-max-allowed=10.11
fi

if [ $VER -eq 6 ]; then
  echo "Configure for OSX 10.6 (libstdc++)"
./configure --prefix=/usr/local --disable-adms \
--with-macosx-sdk=10.6 \
--with-macosx-version-min-required=10.6 \
--with-macosx-version-max-allowed=10.6
fi

# use skd 10.6 with minversion 10.5
if [ $VER -eq 5 ]; then
  echo "Configure for OSX 10.5 (libstdc++)"

./configure --prefix=/usr/local --disable-adms \
--with-macosx-sdk=10.5 \
--with-macosx-version-min-required=10.5 \
--with-macosx-version-max-allowed=10.5
fi


# build
make -j8

# might need to install with sudo, to set correct permissions (?)
# Install to a separate directory for capture.
DEST=/tmp/installdir10$VER

if [ -d $DEST ]; then
  rm -rf $DEST
fi
mkdir $DEST

make install DESTDIR=$DEST

# source location of package resources
SRC=$HOME/git/qucs/contrib/mac

# where to put the resources
mkdir resources

# copy package resources
cp $SRC/qucslogo4_transparent.png resources/
cp $SRC/Welcome.rtf resources/
cp $SRC/License.rtf resources/
cp $SRC/Readme.rtf resources/

# copy locally the postinstall script
mkdir scripts
cp $SRC/postinstall scripts/
chmod +x scripts/postinstall

# copy locally the customized Distribution file
cp $SRC/Distribution.xml .


# pickup DESTDIR and build flat package
rm ./qucs-${version}.pkg
command="pkgbuild \
         --root $DEST \
         --scripts ./scripts
         --identifier org.qucs.pkg \
         --version ${version} \
         --install-location / \
         ./qucs-${version}.pkg"

echo "${command}"
${command}

# check out which Distribution data we have so far
# productbuild --synthesize --package qucs-0.0.19.pkg Distribution.xml
# edit the Distrubution.xml file if necessary

# finish package
productbuild --distribution ./Distribution.xml ./qucs-${version}-OSX10.${VER}.pkg --resources ./resources --package-path ./qucs-${version}.pkg


