[![Build Status](https://travis-ci.org/Qucs/qucs.svg?branch=master)](https://travis-ci.org/Qucs/qucs)
[![Build status](https://ci.appveyor.com/api/projects/status/6wm4mmnk59h7soh1/branch/master?svg=true)](https://ci.appveyor.com/project/qucs/qucs/branch/master)
[![Coverage Status](https://img.shields.io/coveralls/Qucs/qucs.svg)](https://coveralls.io/r/Qucs/qucs?branch=master)

## QUCS - Quite Universal Circuit Simulator


Qucs is an integrated circuit simulator which means you are able to setup a circuit with a graphical user interface (GUI) and simulate the large-signal, small-signal and noise behaviour of the circuit. After that simulation has finished you can view the simulation results on a presentation page or window.

## Resources
  - Website: <http://qucs.sourceforge.net/>
  - Main repository: <http://sourceforge.net/p/qucs/git/>
  - Mirror repository: <https://github.com/Qucs/qucs>
  - Mailing lists: <http://sourceforge.net/p/qucs/mailman/>
  - Forum: <http://sourceforge.net/p/qucs/discussion/>
  - Bug trackers:
    - <http://sourceforge.net/p/qucs/_list/tickets>
    - <https://github.com/Qucs/qucs/issues>
  - Source code documentation:
    - <http://qucs.github.io/qucs-doxygen/qucs/index.html>
    - <http://qucs.github.io/qucs-doxygen/qucs-core/index.html>
  - Downloads: <http://sourceforge.net/projects/qucs/files/>

## Currently Supported Platforms
  - GNU/Linux
  - Darwin/OS X
  - FreeBSD
  - Windows

## Source Download and Compilation

The source code is available as a tarball for stable releases and Git repository clone for development.

The release tarball can be downloaded from: <http://sourceforge.net/projects/qucs/files/qucs/>.
Compilation and install from tarball is expected to work as follows (see dependencies below):

    tar xvfz qucs[version].tar.gz
    cd qucs[version]
    ./configure
    make install

For the lates code in development, clone from one of the Git repositories (the first is updated more frequently):

    git clone git://github.com/Qucs/qucs.git
    git clone git://git.code.sf.net/p/qucs/git

For test and development qucs-test is provided as a Git submodule.
To download the submodule either:

   * clone submodules recursively:
     * `git clone --recursive [repository]`
   * or after cloning, do:
     * `git submodule init`
     * `git submodule update`

Compilation and installation depends on the operation system. See below for an example.

## Compile instructions Linux (Debian/Ubuntu)

First make sure you have all dependencies installed:

    sudo apt-get install build-essential
    sudo apt-get install libqt4-dev libqt4-qt3support
    sudo apt-get install automake libtool gperf flex bison
    sudo apt-get install texlive-font-utils octave-epstk

The ADMS package is necessary. Please [download](https://sourceforge.net/projects/mot-adms/files/adms-source/) the latest tarball and follow the [install](https://github.com/Qucs/ADMS#users-install-from-tarball) instructions. Having`admsXml` on the path should be sufficient.


To build the manuals and user documentation further dependencies are needed. Please check the qucs-doc/README file.


Bootstrap and build everything (after cloning):

    cd qucs
    ./bootstrap
    ./configure
    make
    sudo make install

Compile Qucs GUI tools only:

    cd qucs
    [./bootstrap] #if not executed in the top level
    ./configure
    make
    sudo make install

Compile qucs-core tools only:

    cd qucs-core
    [./bootstrap] #if not executed in the top level
    ./configure
    make
    sudo make install

Compile qucs-doc documentation only:

    cd qucs-doc
    [./bootstrap] #if not executed in the top level
    ./configure
    make
    sudo make install

Testing can be executed at the top level with:

    make check

Note:

 * Qucs will be installed to `/usr/local` by default. You may override this
   by passing `--prefix=[some location]` to the `./configure` script.

 * ADMS should be installed e.g. from a released tarball (>= 2.3.0).
   To use a different `admsXml` pass the option `--with-admsxml=[path/to/]anotherAdmsXml`
   to `./configure'.

 * The LaTex documentation compilation in qucs-doc can be skipped passing `--disable-doc` to the top level `configure` script.

 * Consider the output of `./configure --help` for more and definitive build options.


## Binary Installation

Some of the platform specific pre-compile binary packages containing installers can be found at <https://sourceforge.net/projects/qucs/files/qucs-binary/>.

## License

Qucs is under GNU General Public License, version 2.0+. See COPYING file.
