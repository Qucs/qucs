#!/bin/bash
source gpg_id
echo "using gpg id: $GPG_ID"
#echo "Press any key..."
#read
cd ../..

if [ $# -ne 0 ]
then
  RELEASE=$1
else
  RELEASE=$(date +"%y%m%d")
  RELEASE="0.0.19."${RELEASE:0:6}
fi
echo Building release: $RELEASE

if [ -d release ]
then
	echo Directory release exists, removing ...
	rm -rf release
fi

echo exporting git tree...

git clone ./ release/qucs-$RELEASE
cd release/qucs-$RELEASE
git submodule init
git submodule update


rm -rf .git
#move qucs-doc out and build the pdf's for the qucs-doc debian package
mv qucs-doc ../build_qucs-doc
cd ../build_qucs-doc
./bootstrap
./configure --prefix=$(pwd)/../qucs-doc-$RELEASE
make
make install
cd ..
#extract the 3 folders with pdf files
mv qucs-doc-$RELEASE/share/qucs/docs/* qucs-doc-$RELEASE
rmdir qucs-doc-$RELEASE/share/qucs/docs
rmdir qucs-doc-$RELEASE/share/qucs
rmdir qucs-doc-$RELEASE/share
#copy the debian packaging folder for the debian package
mv qucs-$RELEASE/contrib/ubuntu-debian/qucs-doc/* qucs-doc-$RELEASE/
cd qucs-doc-$RELEASE
./autogen.sh
cd ..

#create the tar archive
tar -zcvhf qucs-doc_$RELEASE.orig.tar.gz qucs-doc-$RELEASE
rm -r qucs-doc-$RELEASE
#extract it again to remove symbolic links
tar -zxvf qucs-doc_$RELEASE.orig.tar.gz 

#copy debian packaging files from contrib into qucs-$RELEASE
mv qucs-$RELEASE/contrib/ubuntu-debian/qucs/debian qucs-$RELEASE
rm -r qucs-$RELEASE/contrib

#Create signed source packages ready to upload to the PPA
DISTS="trusty vivid wily xenial"
tar -zcvhf qucs_$RELEASE.orig.tar.gz qucs-$RELEASE
rm -rf qucs-$RELEASE
tar -zxvf qucs_$RELEASE.orig.tar.gz #make the symbolic links actual files


cd qucs-$RELEASE
COUNT=-0 #last version number in repository
for DIST in ${DISTS} ; do
	COUNT=$(($COUNT-1))
	dch -D $DIST -m -v $RELEASE$COUNT -b snapshot
	debuild -S -k$GPG_ID
done
cd ..

cd qucs-doc-$RELEASE
COUNT=-0 #last version number in repository
for DIST in ${DISTS} ; do
	COUNT=$(($COUNT-1))
	dch -D $DIST -m -v $RELEASE$COUNT -b snapshot
	debuild -S -k$GPG_ID
	./configure
done

