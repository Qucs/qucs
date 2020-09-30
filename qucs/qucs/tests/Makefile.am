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

AM_CPPFLAGS = -I${srcdir}/.. ${QT_CFLAGS} ${QTest_CFLAGS} -DPIC
AM_DEFAULT_SOURCE_EXT = .cpp
# AM_CXXFLAGS = -fPIC
AM_LDFLAGS = -Wl,--no-as-needed

guitests = \
	SchematicTests ElementTests ComponentTests

if COND_WIN32
check_PROGRAMS =
else
check_PROGRAMS = \
	${guitests} \
	model0 \
	model1
endif

model0_CPPFLAGS = ${AM_CPPFLAGS} -I${top_srcdir} -DUNTANGLE_QT
model0_CPPFLAGS += $(QT_CFLAGS) # yikes.
model0_LDADD = ${top_builddir}/qucs/libqucs.la
model0_LDADD += $(X11_LIBS) $(QT_LIBS) # yikes

model1_CPPFLAGS = ${AM_CPPFLAGS} -I${top_srcdir} -DUNTANGLE_QT
model1_CPPFLAGS += $(QT_CFLAGS) # yikes.
model1_LDADD = ${top_builddir}/qucs/libqucs.la
model1_LDADD += $(X11_LIBS) $(QT_LIBS) # yikes


NO_PLUGINS = \
	../components/libcomponents.la \
	../diagrams/libdiagrams.la \
	../paintings/libpaintings.la \
	../dialogs/libdialogs.la

SchematicTests_SOURCES = SchematicTests.cpp SchematicTests.moc.cpp

ElementTests_SOURCES = ElementTests.cpp ElementTests.moc.cpp
ElementTests_CPPFLAGS = ${AM_CPPFLAGS}

ComponentTests_SOURCES = ComponentTests.cpp ComponentTests.moc.cpp
ComponentTests_CPPFLAGS = ${AM_CPPFLAGS}

QUCS_LIBS=../libqucs.la

SchematicTests_LDADD = \
	${QUCS_LIBS} ${PLUGINS} ${QT_LIBS} ${QTest_LIBS}

ElementTests_LDADD = \
	${QUCS_LIBS} ${PLUGINS} ${QT_LIBS} ${QTest_LIBS}

ComponentTests_LDADD = \
	${QUCS_LIBS} ${PLUGINS} ${QT_LIBS} ${QTest_LIBS}

MOCFILES = ${check_PROGRAMS:%=%.moc.cpp}
BUILT_SOURCES = ${MOCFILES}

include ../../common.mk

if COND_WIN32
TESTS =
else
TESTS = ${check_PROGRAMS}
endif

EXTRA_DIST = SchematicTests.h ElementTests.h ComponentTests.h

CLEANFILES = *core *.moc.cpp
