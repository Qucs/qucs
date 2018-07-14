# Copyright (C) 2017 Felix Salfelder
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

examplesdir=$(srcdir)/../examples

AM_DEFAULT_SOURCE_EXT = .cpp
check_LTLIBRARIES = helloworld.la element.la

helloworld_la_LDFLAGS = -shared -no-undefined -module -avoid-version -rpath /dev/null
element_la_LDFLAGS = -shared -no-undefined -module -avoid-version -rpath /dev/null
element_la_CPPFLAGS = ${AM_CPPFLAGS}  $(QT_CFLAGS)

AM_TESTS_ENVIRONMENT = \
    export PATH='../qucs:$(PATH)' \
    export QUCS='qucs' \
    srcdir='$(srcdir)' \
    examplesdir='$(examplesdir)';

if COND_WIN32
helloworld.dll: %.dll: %.la
	[ -f $@ ] || $(LN_S) .libs/$$(. .libs/$<; echo $$dlname) .
helloworld.log: helloworld.dll
element.dll: %.dll: %.la
	[ -f $@ ] || $(LN_S) .libs/$$(. .libs/$<; echo $$dlname) .
element.log: element.dll
else
element.so helloworld.so: %.so: %.la
	[ -f $@ ] || $(LN_S) .libs/$$(. .libs/$<; echo $$dlname) .
helloworld.log: helloworld.so
element.log: element.so
endif

SH_LOG_COMPILER = $(SHELL)
SH_LOG_FLAGS =
TEST_EXTENSIONS = .sh
AM_TESTS_FD_REDIRECT = 9>&2

TESTS = \
	helloworld.sh \
	element.sh \
	doesnotexist.sh \
	netlist.sh

if COND_WIN32
# it does not work on windoze
doesnotexist.log: SH_LOG_FLAGS=-c 'exit 77'
endif

EXTRA_DIST = \
	$(TESTS)

CLEANFILES = *.out *core
