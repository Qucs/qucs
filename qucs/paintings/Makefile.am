## Process this file with automake to produce Makefile.in
# 
# qucs/paintings/Makefile.am
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
# the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
# Boston, MA 02110-1301, USA.  
#
#
include MakeList

#private library
qucsdir=${pkglibdir}

qucs_LTLIBRARIES = libpaintings.la

MOCHEADERS = arrowdialog.h graphictextdialog.h filldialog.h id_dialog.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

libpaintings_la_CXXFLAGS = ${AM_CXXFLAGS} -fPIC
include ../../common.mk

libpaintings_la_SOURCES = ${SRCS}


nodist_libpaintings_la_SOURCES = $(MOCFILES)

# noinst_HEADERS = $(MOCHEADERS) graphictext.h  \
#   paintings.h rectangle.h ellipsearc.h id_text.h

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/qucs

CLEANFILES = *~ *.moc.cpp
MAINTAINERCLEANFILES = Makefile.in
