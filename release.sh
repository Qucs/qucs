#!/bin/bash


if [ $# -ne 0 ]
then
  RELEASE=$1
else
  RELEASE=$(date +"%y%m%d")
  RELEASE="0.0.17."${RELEASE:0:6}
fi
echo Building release: $RELEASE

if [ -d release ]
then
	echo Directory release exists, removing ...
	rm -rf release
fi

echo exporting git tree...
WINDIR=$PWD/qucs-win32-bin
git clone ./ release/
mv release/qucs-core release/qucs/
mv release/qucs release/qucs-$RELEASE
rm -rf release/.git


#Include the asco archive
cd release/qucs-$RELEASE
wget https://downloads.sourceforge.net/project/asco/asco/0.4.8/ASCO-0.4.8.tar.gz
tar -zxvf ASCO-0.4.8.tar.gz
rm ASCO-0.4.8.tar.gz
mv ASCO-0.4.8 asco
cd asco
tar -zxvf Autotools.tar.gz
./autogen.sh
cd ..

#include the freehdl archive
wget http://freehdl.seul.org/~enaroska/freehdl-0.0.7.tar.gz
tar -zxvf freehdl-0.0.7.tar.gz
rm freehdl-0.0.7.tar.gz
mv freehdl-0.0.7 freehdl

sed -i 's/# AC_CONFIG_SUBDIRS(qucs-core)/AC_CONFIG_SUBDIRS(qucs-core)/g' configure.ac
sed -i 's/# RELEASEDIRS="qucs-core"/RELEASEDIRS="qucs-core"/g' configure.ac
sed -i 's/# AC_CONFIG_SUBDIRS(freehdl)/AC_CONFIG_SUBDIRS(freehdl)/g' configure.ac
sed -i 's/# RELEASEDIRS="$RELEASEDIRS freehdl"/RELEASEDIRS="$RELEASEDIRS freehdl"/g' configure.ac
sed -i 's/# AC_CONFIG_SUBDIRS(asco)/AC_CONFIG_SUBDIRS(asco)/g' configure.ac
sed -i 's/# RELEASEDIRS="$RELEASEDIRS asco"/RELEASEDIRS="$RELEASEDIRS asco"/g' configure.ac

exit
./autogen.sh
make distclean
rm -rf autom4te.cache


cd qucs-core
./autogen.sh
make
./configure
make distclean
rm -rf autom4te.cache
cd ..
cd ..

echo creating source archive...

tar -zcvf qucs-$RELEASE.tar.gz qucs-$RELEASE

DISTS="precise quantal"

cp qucs-$RELEASE.tar.gz qucs_$RELEASE.orig.tar.gz

cd qucs-$RELEASE
COUNT=-0 #last version number in repository
for DIST in ${DISTS} ; do
	COUNT=$(($COUNT-1))
	dch -D $DIST -m -v $RELEASE$COUNT -b
	debuild -S #-k8AD5905E
	./configure 
done


echo "Building mingw32"
make clean
QTDIR=~/.wine/drive_c/Qt/4.8.4/ ./mingw-configure --prefix=$WINDIR
sed -i 's/-fno-rtti/ /g' qucs-filter-v2/Makefile
make
make install

