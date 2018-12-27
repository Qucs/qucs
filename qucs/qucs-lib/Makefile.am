## Process this file with automake to produce Makefile.in
#
# qucs-lib/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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

include ../common.mk

SUBDIRS = library

bin_PROGRAMS = qucslib

dist_man_MANS = qucslib.1

MOCHEADERS = qucslib.h displaydialog.h symbolwidget.h librarydialog.h \
       qucslib_common.h

MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

qucslib_SOURCES = main.cpp qucslib.cpp displaydialog.cpp symbolwidget.cpp \
       librarydialog.cpp qucslib_.qrc

nodist_qucslib_SOURCES = $(MOCFILES)
nodist_qucslib_CXXFLAGS = ${AM_CXXFLAGS} -fPIC

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS) -I$(top_srcdir)/qucs
qucslib_LDFLAGS = $(X11_LDFLAGS) $(QT_LIBS)
qucslib_LDADD = $(X11_LIBS) $(QT_LIBS)

noinst_HEADERS = $(MOCHEADERS)

CLEANFILES = *~ qucslib_.cpp
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
EXTRA_DIST = bitmaps/home.png bitmaps/big.qucs.xpm
