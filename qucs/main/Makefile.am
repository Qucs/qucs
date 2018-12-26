## Process this file with automake to produce Makefile.in
# 
# Makefile.am
#
# Automake input file.
#
# Copyright (C) 2016-18 Felix Salfelder
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

qucs_appledir = $(bindir)/qucs.app/Contents/MacOS/

include ../common.mk

all-local: qucs

# why this? qucs_real_LDADD does not seem to work on windows (yet?)

qucs_real_LDFLAGS = $(QT_LDFLAGS)
qucs_real_SOURCES = main.cpp # qucs_.qrc
qucs_real_CPPFLAGS = ${AM_CPPFLAGS} $(QT_CFLAGS) -I${top_srcdir}/qucs\
		-DQUCS_PLUGPATH=\"${pkglibdir}\"

if COND_WIN32
qucs_real_LDADD = \
	../qucs/libqucs.la
else
qucs_real_LDADD = \
	../qucs/libqucs.la \
	$(X11_LIBS) $(QT_LIBS)
endif

if COND_MACOSX
qucs_apple_PROGRAMS = qucs.real
else
endif

bin_PROGRAMS = qucs.real
transform=s&\.real$$&&;$(program_transform_name)

EXTRA_DIST = \
	io_findf.cc \
	README

CLEANFILES = qucs_.cpp
