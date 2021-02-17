## Process this file with automake to produce Makefile.in
#
# stuff.am
#
# rules used in other makefiles
#
# Author: Felix
# Copyright (C) 2017 QUCS team
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

include MakeList

all: ${TARGET:%=_}

# FIXME: this is platform dependent.
SOEXT=.so

_: | ${TARGET}.la
	test -L _ || ${LN_S} .libs/${TARGET}${SOEXT} $@


SUFFIXES = .qrc

.qrc.cpp:
	$(RCC) -o $@ $<


# TODO: does not make sense anymore.
$(MOCFILES): $(top_builddir)/qt_version

AM_CXXFLAGS = ${CXXFLAGS} -fPIC
AM_CPPFLAGS = -I$(top_srcdir)/include

# BUG
AM_CPPFLAGS += -I$(top_srcdir)/src
EXTRA_DIST += MakeList

# simplify paths
here_rel = $(subst ${abs_top_srcdir},,${abs_srcdir})
instdir = $(pkglibdir)/$(here_rel)/..

# TODO: drop -I[..]/src
.h.cc:
	$(MOC) -I${top_srcdir}/include -I${top_srcdir}/src -DQT_MAJOR_VERSION=${QT_MAJOR_VERSION} -o $@ $< # 2
