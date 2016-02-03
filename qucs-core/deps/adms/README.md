
[![Build Status](https://travis-ci.org/Qucs/ADMS.svg?branch=master)](https://travis-ci.org/Qucs/ADMS)

# ADMS - An automatic device model synthesizer

ADMS is a code generator that converts electrical compact device models specified
in high-level description language into ready-to-compile C code for the API of spice
simulators. Based on transformations specified in XML language, ADMS transforms
Verilog-AMS code into other target languages.

This version is forked from the code previously available at:
<http://sourceforge.net/projects/mot-adms/files/adms-source/2.3/>

The original SVN repo no longer available.

This version is based on ADMS 2.3.0, the last version released by the original author.

Currently only a subset of the Verilog-AMS language is supported.

- Homepage (unmaintained): <http://vacomp.noovela.com/>
- Source code: <http://sourceforge.net/projects/mot-adms/>

## Platforms

ADMS is known to work on these platforms.

- GNU/Linux
- Darwin/OS X
- FreeBSD
- Windows


## Dependencies

- C compiler (gcc, clang)
- Autotools (autoconf, automake,...) or CMake
- GNU Flex
- GNU Bison 2.6+
- GNU Libtool
- Perl with XML::LibXml and GD modules (only for maintainers)


## Installation

Autotools is the original build system and is more complete. Prefered for maintainance and packaging.
CMake was recently introduced and is not as well tested.


### Maintainers Install and Packaging from Git

This section is relevant in case ADMS is compiled from the Git repository.

#### Compilation Using Autotools

The `--enable-maintainer-mode` makes sure the required files are generated (using Perl and LibXml).

    sh bootstrap.sh
    ./configure --enable-maintainer-mode
    make install

Autotools it currently used for creating release packages, the `adms-x.x.x.tar.gz` source code archive.

    sh bootstrap.sh
    ./configure --enable-maintainer-mode
    make clean
    make dist

#### Compilation Using CMake

The `-DUSE_MAINTAINER_MODE=ON` makes sure the required files are generated (using Perl and LibXml).

    mkdir cmake; cd cmake
    cmake .. -DUSE_MAINTAINER_MODE=ON -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/install/location/
    make install

Packaging is not yet supported with CMake.


### Users Install from Tarball

This section is relevant in case ADMS is compiled from a source code archive (`adms-x.x.x.tar.gz`).
Users should be able to build without Perl (and the maintainer required modules XML::LibXml and GD).

#### Compilation Using Autotools

    tar xvfz adms-x.x.x.tar.gz
    cd adms-x.x.x
    ./configure --prefix=[/install/location/]
    make install

#### Compilation Using CMake

    tar xvfz adms-x.x.x.tar.gz
    cd adms-x.x.x
    mkdir build; cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=[/install/location/]
    make install


## Credits

See AUTHORS file.

## License

ADMS is under LGPL-GNU Lesser General Public License, version 2.1. See COPYING file.



---

Original author's build instructions: (for posterity)

```

0- Prerequisites
    0- gnu make is required (do not use other native make)

1- Description
  ADMS is a code generator that converts electrical compact device models
  specified in high-level description language into ready-to-compile c code
  for the API of spice simulators.

2- Installation - Unix or Linux
    0- run: ./configure - see file INSTALLATION for more options
    1- run: gmake
    2- run: gmake install
  If the installation fails try:
    0- run: autoheader
    1- run: aclocal
    2- run: automake
    3- run: autoconf
    4- run: ./configure - see file INSTALLATION for more options
    5- run: gmake
    6- run: gmake install
    In two shots:
      autoheader && aclocal && automake && autoconf && ./configure
      gmake && gmake install

3- Installation - Windows with cygwin environment
    0- run: ./configure - see file INSTALLATION for more options
    1- run: gmake
    2- run: gmake install
  If the installation fails try:
    0- run: autoheader
    1- run: aclocal
    2- run: automake
    3- run: autoconf
    4- run: ./configure - see file INSTALLATION for more options
    5- run: gmake
    6- run: gmake install
    In two shots:
      autoheader && aclocal && automake && autoconf && ./configure
      gmake && gmake install

4- Installation windows (no cygwin environment)
  0- run:
       cl -DYY_NO_UNISTD_H -DPACKAGE_VERSION=\"msvc-229\" -DHAVE_CONFIG_H -DinsideElement -DinsidePreprocessor -DinsideVeriloga -I. -IadmsXml \
          admsXml/adms.c admsXml/preprocessorLex.c admsXml/preprocessorMain.c admsXml/preprocessorYacc.c admsXml/verilogaLex.c admsXml/verilogaYacc.c \
          admsXml/admsXml.c admsXml/admstpathYacc.c -o admsXml.exe
     If the installation fails send a message to r29173@users.sourceforge.net
     (note that there is NO strong support for windows compilers.)

5- Directory Structure
   adms
    - metaadms.dtd
      DTD of file adms.xml
    - adms.xml
      This file defines the data structure used by adms to save parsed hdl code.
    - auxconf
      set-up files created by automake -a -c
    - scripts:
      perl scripts
    - admsXml
      source code

5- For developers (using the cygwin environment)
  Here is how I create the compilation environment after checking out adms from
  the CVS repository:
  0- run: autoheader (create autom4te.cache/config-h.in from configure.in)
  1- run: mkdir auxconf (create directory auxconf)
  2- run: aclocal (create aclocal.m4 from configure.in)
  3- run: libtoolize --force --ltdl -c (create libltdl and config.guess, config.sub, ltmain.sh in auxconf)
  4- run: automake -a -c (create missing, mkinstalldirs, install-sh in auxconf and all Makefile.in)
  5- run: autoconf (create configure)
  6- run: ./configure --enable-maintainer-mode
  In three shots:
    rm -rf auxconf && autoheader && mkdir auxconf && aclocal && libtoolize --force --ltdl -c
    touch ChangeLog && automake -a -c && autoconf
    ./configure --enable-maintainer-mode
```
