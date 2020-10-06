## Process this file with automake to produce Makefile.in
#
# Automake input file.
#
# Copyright (C) 2018 Felix Salfelder
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
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

plugindir = ${pkglibdir}
plugin_LTLIBRARIES = vadim-components.la

vadim_components_la_LDFLAGS = -shared -module -avoid-version
vadim_components_la_SOURCES = \
	DIODE_SPICE.cpp

vadim_components_la_CXXFLAGS = ${AM_CXXFLAGS} -fPIC

# BUG: components.h still uses Q3Ptrlist from qt4
vadim_components_la_CPPFLAGS = ${AM_CPPFLAGS} $(QT_CFLAGS)

AM_CPPFLAGS = \
		-I${top_srcdir}/include \
		-I$(top_srcdir)/qucs

CLEANFILES = *~ Makefile.in
