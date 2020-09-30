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

pkglib_LTLIBRARIES = legacy.la
AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS) -I$(top_srcdir)/qucs
AM_CXXFLAGS = ${CXXFLAGS} -fPIC

legacy_la_LDFLAGS = -module -avoid-version
legacy_la_CPPFLAGS = $(AM_CPPFLAGS)
# need Qt for TextStream.
legacy_la_CPPFLAGS += $(QT_INCLUDES)
legacy_la_SOURCES = \
	rectdiagram.cpp \
	curvediagram.cpp \
	markerdialog.cpp \
	polardiagram.cpp \
	psdiagram.cpp \
	rect3ddiagram.cpp \
	smithdiagram.cpp \
	tabdiagram.cpp \
	timingdiagram.cpp \
	truthdiagram.cpp \
	schematic_format.cpp \
	schematic_lang.cpp \
	subcircuit.cpp \
	simulator.cpp
# phasordiagram.cpp
# waveac.cpp

noinst_HEADERS = \
	markerdialog.h
	marker.h
