## Process this file with automake to produce Makefile.in
#
# Makefile.am
#
# Automake input file.
#
# Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this package; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
# Boston, MA 02110-1301, USA.
#

## if maintainer mode we may build adms
if MAINTAINER_MODE
## Test if an internal adms has been requested
if USE_INTERNAL_ADMS
SUBDIRS = deps/adms
else
SUBDIRS =
endif
else
SUBDIRS =
endif

## Add the subdirectories, including a '.' which forces adms
## to be build first, followed by src, then doc
SUBDIRS += src doc .

# include subdirs to build gtest and run unit tests
if USE_GTEST
SUBDIRS += $(GTEST_DIR) tests
endif

ACLOCAL_AMFLAGS = -I m4

nodist_pkginclude_HEADERS = qucs_typedefs.h

EXTRA_DIST = BUGS bootstrap.sh depcomp RELEASE

pkginclude_HEADERS = config.h

CLEANFILES = *~ *.orig *.rej
MAINTAINERCLEANFILES = aclocal.m4 config.h.in configure Makefile.in \
	stamp-h.in stamp-h[0-9].in
DISTCLEANFILES = config.cache config.log

## Test suite
#  Run with: make check
# These tests rely on the assert and bugon functions.
# The functions are used as equations on the schematic.
# Fail on these functions will abort the simulation of netlist generated
# from the schematic.
#
# See also qucs-test for another way of testing.
# https://github.com/Qucs/qucs-test.
TESTS =                                               \
  tests/basic/u=ri/u=ri@dc.net                        \
  tests/basic/u=ri/u=ri@ac.net                        \
  tests/basic/u=ri/u=ri@tr.net                        \
  tests/basic/voltagediviser/voltagediviser@tr.net

TEST_EXTENSIONS = .net
NET_LOG_COMPILER = src/qucsator
AM_NET_LOG_FLAGS = -i


install-exec-hook:
	@echo "----------------------------------------------"
	@echo "                    NOTE                      "
	@echo "On some systems (e.g. Linux) you may need"
	@echo "to run sudo ldconfig or reboot before running"
	@echo "qucsator or trying to use the libraries it"
	@echo "installs. Otherwise you will receive an error"
	@echo "about loading the shared libraries."
	@echo "----------------------------------------------"

