#!/bin/bash

## file: package.sh
#
## This script create a tarball from the Qucs source repository.
#
## Run this script from: ~/git/qucs/
## Outputs in:           ~/git/qucs/release/


## Package contains sources for:
#  * Qucs, Qucsfilter... (Qt GUI components)
#  * Qucs-core (backend)
#  * Qucs-doc (only the generated PDF documents)
#  * Examples
#  * ASCO 0.4.9 (patched version)
#  * ADMS 2.3.2+ (git submodule, might not be the latest stable release)


## Note:
# * ADMS is optional. The needed sources are already generated.
# * ADMS is needed to use the Verilog-A dynamic loader.
# * ASCO is also optional.


##
# Relevant options for packagers:
# * --disable-asco , skip build of shipped ASCO [default no]
# * --disable-adms , skip build of shipped ADMS [default no]





# Pass argument with version of stable
if [ $# -ne 0 ]
then
  RELEASE=$1
else
  # Set Release git tag
  # TODO get it from git tags or VERSION file
  RELEASE=$(date +"%y%m%d")
  RELEASE="0.0.19-"${RELEASE:0:6}

  # Get Git short hash
  GIT=$(git log --pretty=format:'%h' -n 1)
  # Append git short hash
  RELEASE=${RELEASE}-git-${GIT}

fi


echo =====================
echo Building release: $RELEASE
echo =====================

REPO=${PWD}
echo =====================
echo Working from: ${REPO}
echo =====================

# TODO test location
if [ -d release ]
then
	echo Directory release exists, removing ...
	rm -rf release
fi

echo =================================
echo Exporting git tree recursively...
echo =================================

# Clone only Qucs code (ADMS version handled below)
git clone --recursive ./ release/

#checkout ADMS version
cd release/qucs-core/deps/adms/ && git checkout -b release-2.3.4 && cd -

# flatten subdiretories
mv release/examples release/qucs/examples/examples
mv release/qucs-core release/qucs/

# TODO remove .gitignore, .travis, ...

# tag directory
echo =====================
echo Release directory: $REPO/release/qucs-$RELEASE
echo =====================
mv release/qucs release/qucs-$RELEASE
rm -rf release/.git
rm -r release/qucs-$RELEASE/qucs-core/deps/adms/.git

# release broke CMake! Release has different structure than Repository
#find release/qucs-$RELEASE -name CMakeLists.txt | xargs rm
#rm -rf release/qucs-$RELEASE/cmake
#rm -rf release/qucs-$RELEASE/qucs-core/cmake

echo =====================================
echo Build PDF documentation in source dir
echo =====================================
cd $REPO/release/qucs-doc
./autogen.sh

cd tutorial
make tutorial
make book
cd ..

cd report
make report
make book
cd ..

cd technical
make technical
#ps2pdf technical.ps
cd ..


echo =================================================
echo Including pdf versions of qucs-doc to release dir
echo =================================================
cd $REPO/release/qucs-doc
DOC_SUBDIRS="report technical tutorial"
for DOC_SUBDIR in ${DOC_SUBDIRS} ; do
	cd $DOC_SUBDIR
	mkdir -p $REPO/release/qucs-$RELEASE/qucs-doc/$DOC_SUBDIR
	find . -name "*.pdf" |grep -v pics| xargs -I {} cp {} $REPO/release/qucs-$RELEASE/qucs-doc/$DOC_SUBDIR
	cd ..
done
# done with qucs-doc
cd $REPO/release
rm -rf qucs-doc


echo ============================================
echo Cleanup qucs-doc and examples in release dir
echo ============================================
cd $REPO/release/qucs-$RELEASE
cd qucs-doc
./autogen.sh
make distclean
cd ..

cd examples
./autogen.sh
make distclean
cd ..


echo ===============================================
echo Download, patch and include ASCO to release dir
echo ===============================================
cd $REPO/release/qucs-$RELEASE
if [ -f ~/Downloads/ASCO-0.4.9.tar.gz ]
then
	cp ~/Downloads/ASCO-0.4.9.tar.gz .
else
	wget https://downloads.sourceforge.net/project/asco/asco/0.4.9/ASCO-0.4.9.tar.gz -P ~/Downloads/
fi

# Extract ASCO tarball, patch and prepare build system to redistribute
tar -zxvf ASCO-0.4.9.tar.gz
rm ASCO-0.4.9.tar.gz
mv ASCO-0.4.9 asco
cd asco
patch -p1 < ../../../qucs/contrib/patch_asco_unbuffer.diff
#https://sourceforge.net/p/qucs/bugs/143/
patch -p1 < ../../../qucs/contrib/asco-nmlatest.c.patch
touch NEWS
tar -zxvf Autotools.tar.gz
# force LINUX definition while building on OSX
patch -p1 < ../../../qucs/contrib/patch_asco_osx.diff
# add default disable MPI (issue cross compiling)
patch -p1 < ../../../qucs/contrib/asco-default-disabled-MPI-enable-mpi-option.patch
./autogen.sh
automake --add-missing
aclocal
cd ..


#echo ========================
#echo Checkout ADMS submodule
#echo ========================
#cd $REPO/release/qucs-$RELEASE
#cd qucs-core/deps/adms
#./bootstrap.sh  # skip bootstrap done by qucs-core
#./configure --enable-maintainer-mode
#make clean
#make dist
#rm -r adms-*.tar.gz

#echo =================================
#echo Download and extract ADMS tarball
#echo =================================
#cd $REPO/release/qucs-$RELEASE
#if [ -f ~/Downloads/adms-2.3.4.tar.gz ]
#then
#	cp ~/Downloads/adms-2.3.4.tar.gz .
#else
#  wget http://sourceforge.net/projects/mot-adms/files/adms-source/2.3/adms-2.3.4.tar.gz -P ~/Downloads/
#fi
#tar -zxvf adms-2.3.4.tar.gz
#mv adms-2.3.4 adms
#mv adms/* qucs-core/deps/adms/
#rm -r adms/


echo ==================================
echo Set configure.ac into RELEASE mode
echo ==================================
cd $REPO/release/qucs-$RELEASE
sed -i '' 's/RELEASE=no/RELEASE=yes/g' configure.ac


echo =========================================
echo Bootstrap Qucs, Qucs-edit, Qucs-filter...
echo =========================================
cd $REPO/release/qucs-$RELEASE
./autogen.sh
#make clean #no need
rm -rf autom4te.cache
#libtoolize
case `uname` in
  *Darwin*) LIBTOOLIZE=glibtoolize ;;
  *)        LIBTOOLIZE=libtoolize ;;
esac
$LIBTOOLIZE


echo ===================
echo Bootstrap Qucs-core
echo ===================
cd qucs-core
./bootstrap.sh


# TODO find a more elegant way to handle ADMS as a subproject, AC_CONFIG_SUBDIRS maybe?

# fix libtool complaining
cd deps/adms/
#libtool: Version mismatch error.  This is libtool 2.4.5, but the
#libtool: definition of this LT_INIT comes from libtool 2.4.2.
#libtool: You should recreate aclocal.m4 with macros from libtool 2.4.5
#libtool: and run autoconf again.
glibtoolize && autoconf ## complaining about libtool versions (!)
cd -

./configure --enable-maintainer-mode
make       # build maintainer-mode Verilog-A C++ sources
make clean # remove object code; this also removes the maintainer-mode files from the ADMS submodule (BUG!)
#./configure
rm -rf autom4te.cache

# bootstrap ADMS once again
cd deps/adms/
./bootstrap.sh
##glibtoolize && autoconf ## complaining about libtool versions (!)
./configure --enable-maintainer-mode
#make
make dist
rm -r adms-*.tar.gz


echo ==========================
echo Creating source archive...
echo ==========================
cd $REPO/release/
tar -zcvhf qucs-$RELEASE.tar.gz qucs-$RELEASE
#rm -rf qucs-$RELEASE
tar -zxvf qucs-$RELEASE.tar.gz #make the symbolic links actual files


echo +++++++++++++++
echo Saving copy to: ${HOME}/Downloads/qucs-$RELEASE.tar.gz
echo +++++++++++++++
cp qucs-$RELEASE.tar.gz ${HOME}/Downloads/



