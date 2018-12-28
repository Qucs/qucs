## Process this file with automake to produce Makefile.in
# 
# qucs/dialogs/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004, 2006, 2009 Stefan Jahn <stefan@lkcc.org>
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
qucsdir=${pkglibdir}

# qucs_LTLIBRARIES = libdialogs.la


MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

include ../../common.mk


libdialogs_la_CXXFLAGS = ${AM_CXXFLAGS} -fPIC
libdialogs_la_SOURCES =

if COND_WIN32
libdialogs_la_LIBADD = ../libqucsschematic.la
else
libdialogs_la_LIBADD = \
	$(X11_LIBS) \
	$(QT_LIBS)
endif

nodist_libdialogs_la_SOURCES = $(MOCFILES)

noinst_HEADERS = $(MOCHEADERS) $(UIHEADERS)

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS) -I$(top_srcdir)/qucs



CLEANFILES = *~ *.moc.cpp ui_*.h
MAINTAINERCLEANFILES = Makefile.in
