## Process this file with automake to produce Makefile.in
#
# qucs-attenuator/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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

include ../../autotools/common.mk

bin_PROGRAMS = qucsattenuator

SUBDIRS = bitmaps

dist_man_MANS = qucsattenuator.1

MOCHEADERS = qucsattenuator.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

qucsattenuator_SOURCES = ${SRCS} ${RESOURCES}
qucsattenuator_CXXFLAGS = ${AM_CXXFLAGS} -fPIC

nodist_qucsattenuator_SOURCES = $(MOCFILES)

AM_CPPFLAGS += $(X11_INCLUDES) $(QT_CFLAGS)
qucsattenuator_LDFLAGS = $(X11_LDFLAGS) $(QT_LIBS)
qucsattenuator_LDADD = $(X11_LIBS) $(QT_LIBS)

noinst_HEADERS = $(MOCHEADERS) attenuatorfunc.h

CLEANFILES = *~ *_.cpp qucsattenuator.moc.cpp
