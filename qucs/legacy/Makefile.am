## Process this file with automake to produce Makefile.in
#
# Automake input file.
#
# Copyright (C) 2015 Felix Salfelder felix@salfelder.org
# Copyright (C) 2018 Felix Salfelder felix@salfelder.org
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 8, or (at your option)
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

include ../../common.mk
include MakeList

pkglib_LTLIBRARIES = legacy.la legacylib.la qucsator.la
AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/qucs
# AM_CXXFLAGS = ${CXXFLAGS} -fPIC # common?

legacy_la_LDFLAGS = -module -avoid-version
legacy_la_CPPFLAGS = $(AM_CPPFLAGS)
# need Qt for TextStream.
legacy_la_CPPFLAGS += $(QT_INCLUDES)
legacy_la_SOURCES = ${LEGACY_SRC}

legacylib_la_LDFLAGS = -module -avoid-version
legacylib_la_CPPFLAGS = $(AM_CPPFLAGS)
# need Qt for TextStream.
legacylib_la_CPPFLAGS += $(QT_INCLUDES)
legacylib_la_SOURCES = \
	libfiles.cpp

qucsator_la_SOURCES = qucsator.cpp dat.cpp qucsator.moc.cpp

qucsator_la_LDFLAGS = -module -avoid-version
# qucsator_la_CPPFLAGS = $(AM_CPPFLAGS)
# need Qt for TextStream. (BUG?)
qucsator_la_CPPFLAGS = $(AM_CPPFLAGS) $(QT_INCLUDES)
# qucsator_la_LIBADD = $(QT_LIBS)

noinst_HEADERS = $(HDR)
