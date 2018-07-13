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

SUFFIXES = .qrc

.qrc.cpp:
	$(RCC) -o $@ $<

SUFFIXES += .moc.cpp

.h.moc.cpp:
	$(MOC) -o $@ $<

$(MOCFILES): $(top_builddir)/qt_version
