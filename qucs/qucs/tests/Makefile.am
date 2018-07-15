# Copyright (C) 2018 Felix Salfelder
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

AM_DEFAULT_SOURCE_EXT = .cpp
check_PROGRAMS = SchematicTests
SchematicTests_SOURCES = SchematicTests.cpp SchematicTests.moc.cpp

SchematicTests_LDADD = \
	../libqucsschematic.la ${QT_LIBS} ${QTest_LIBS}

MOCFILES = SchematicTests.moc.cpp
BUILT_SOURCES=${MOCFILES}

include ../../common.mk

AM_CPPFLAGS = -I${srcdir}/.. ${QT_CFLAGS} ${QTest_CFLAGS}

TESTS = ${check_PROGRAMS}
EXTRA_DIST=SchematicTests.h
