## Process this file with automake to produce Makefile.in
# 
# qucs/diagrams/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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

# private library
qucsdir = ${pkglibdir}

qucs_LTLIBRARIES = diagrams.la

include ../../common.mk

# MOCHEADERS = diagramdialog.h  markerdialog.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

diagrams_la_CXXFLAGS = ${AM_CXXFLAGS} -fPIC
diagrams_la_LDFLAGS = -module -avoid-version

diagrams_la_SOURCES = \
	rectdiagram.cpp

NOTYET = \
	curvediagram.cpp \
	markerdialog.cpp \
	phasordiagram.cpp \
	polardiagram.cpp \
	psdiagram.cpp \
	rect3ddiagram.cpp \
	smithdiagram.cpp \
	timingdiagram.cpp \
	truthdiagram.cpp \
	waveac.cpp

nodist_diagrams_la_SOURCES = $(MOCFILES)

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/qucs

CLEANFILES = *~ *.moc.cpp
MAINTAINERCLEANFILES = Makefile.in
