## Process this file with automake to produce Makefile.in
#
# qucs-widebandmatching/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2017 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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

bin_PROGRAMS = QucsWidebandMatching

MOCHEADERS = ui.h io.h MathOperations.h mat.h sparengine.h GRABIM.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

QucsWidebandMatching_SOURCES = main.cpp ui.cpp io.cpp MathOperations.cpp mat.cpp sparengine.cpp GRABIM.cpp qrc_QucsWidebandMatching.cpp

EXTRA_DIST = QucsWidebandMatching.qrc

qrc_QucsWidebandMatching.cpp: QucsWidebandMatching.qrc
	$(RCC) -o $@ $<


nodist_QucsWidebandMatching_SOURCES = $(MOCFILES)

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_INCLUDES) 
QucsWidebandMatching_LDFLAGS = $(X11_LDFLAGS) $(QT_LDFLAGS)
QucsWidebandMatching_LDADD = $(X11_LIBS) $(QT_LIBS)

noinst_HEADERS = $(MOCHEADERS)

SUFFIXES = .moc.cpp

.h.moc.cpp:
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
