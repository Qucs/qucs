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

# Ignore external headers and sources,
# enable branch coverage (default disabled on lcov 1.10)
CODE_COVERAGE_LCOV_OPTIONS ?= --no-external --rc lcov_branch_coverage=1 $(CODE_COVERAGE_LCOV_OPTIONS_DEFAULT)
CODE_COVERAGE_GENHTML_OPTIONS ?= --rc lcov_branch_coverage=1
@CODE_COVERAGE_RULES@

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
MOSTLYCLEANFILES = \
    *.gcda *.gcno *.gcov

## Test suite
#  Run with: make check
# These tests rely on the assert and bugon functions.
# The functions are used as equations on the schematic.
# Fail on these functions will abort the simulation of netlist generated
# from the schematic.
#
# See also qucs-test for another way of testing.
# https://github.com/Qucs/qucs-test.
TEST_EXTENSIONS = .net .txt
NET_LOG_COMPILER = $(top_srcdir)/tests/runqucsator.sh
AM_NET_LOG_FLAGS = $(abs_top_srcdir)/src/qucsator

TESTS =                                               \
  tests/basic/u=ri/u=ri@dc.net                        \
  tests/basic/u=ri/u=ri@ac.net                        \
  tests/basic/u=ri/u=ri@tr.net                        \
  tests/basic/u=ri/u=ri@dc+sweep.net                  \
  tests/basic/u=ri/u=ri@ac+sweep.net                  \
  tests/basic/u=ri/u=ri@tr+sweep.net                  \
  tests/basic/voltagediviser/voltagediviser@tr.net

if USE_QUCS_TEST
    TXT_LOG_COMPILER = src/qucsator
    AM_TXT_LOG_FLAGS = -i
    TESTS +=  tests/qucs-test/testsuite/DC_SW_bridge_prj/netlist.txt \
              tests/qucs-test/testsuite/DC_SW_bsim4v30nMOS_Ids_Vgs_prj/netlist.txt \
              tests/qucs-test/testsuite/DC_SW_bsim4v30pMOS_Ids_Vgs_prj/netlist.txt \
              tests/qucs-test/testsuite/DC_SW_charac_prj/netlist.txt \
              tests/qucs-test/testsuite/DC_SW_diff1_prj/netlist.txt \
              tests/qucs-test/testsuite/DC_TR_SW_spice_BFR520_prj/netlist.txt
endif
