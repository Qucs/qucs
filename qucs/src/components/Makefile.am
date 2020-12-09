## Process this file with automake to produce Makefile.in
#
# qucs/components/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
#               2013 addded Mike Brinson  mbrin72043@yahoo.co.uk
#               2018, 2020 Felix Salfelder
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

# private library
qucsdir=${pkglibdir}

include ../../common.mk

plugindir = ${pkglibdir}
plugin_LTLIBRARIES = qucs-default-components.la

qucs_default_components_la_LDFLAGS = -shared -module -avoid-version

include MakeList

qucs_default_components_la_SOURCES = $(DEFAULT_COMPONENTS)
qucs_default_components_la_LIBADD = $(QT_LIBS)
qucs_default_components_la_CXXFLAGS = ${AM_CXXFLAGS} -fPIC

# some stuff mostly related to legacy components
qucs_default_components_la_SOURCES += \
	${TASK_ELEMENTS} \
	${LUMPED_COMPONENTS} \
	${RF_COMPONENTS} \
	$(OTHERCOMPONENTS)

noinst_HEADERS = 	$(MOCHEADERS) \
			$(HEADERS_)

AM_CPPFLAGS += $(X11_INCLUDES) $(QT_CFLAGS)

# BUG
AM_CPPFLAGS += -I$(top_srcdir)/src

CLEANFILES = *~ *.moc.cpp
MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = MakeList
