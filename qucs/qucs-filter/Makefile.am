## Process this file with automake to produce Makefile.in
#
# qucs-filter/Makefile.am
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

bin_PROGRAMS = qucsfilter

MOCHEADERS = qucsfilter.h helpdialog.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

qucsfilter_SOURCES = main.cpp qucsfilter.cpp helpdialog.cpp

nodist_qucsfilter_SOURCES = $(MOCFILES)

INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES)
qucsfilter_LDFLAGS = $(X11_LDFLAGS) $(QT_LDFLAGS)

noinst_HEADERS = $(MOCHEADERS)

%.moc.cpp: %.h
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
