## Process this file with automake to produce Makefile.in
# 
# qucs/bitmaps/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.  
#

noinst_LIBRARIES = libdiagrams.a

MOCHEADERS = diagramdialog.h markerdialog.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

libdiagrams_a_SOURCES = tabdiagram.cpp smithdiagram.cpp rectdiagram.cpp       \
  polardiagram.cpp graph.cpp diagramdialog.cpp diagram.cpp marker.cpp \
  markerdialog.cpp $(MOCFILES)

noinst_HEADERS = $(MOCHEADERS) diagram.h graph.h polardiagram.h rectdiagram.h \
  smithdiagram.h tabdiagram.h diagrams.h marker.h

INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES)

%.moc.cpp: %.h
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
