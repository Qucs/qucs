## Process this file with automake to produce Makefile.in
#
# qucs/sim/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2015, 2019 Felix Salfelder felix@salfelder.org
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

pkglib_LTLIBRARIES = verilog.la qucsator.la
noinst_HEADERS = sim.h
AM_DEFAULT_SOURCE_EXT = .cpp
AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS) -I$(top_srcdir)/qucs

verilog_la_SOURCES = verilog.cpp verilog_nl.cpp
verilog_la_LDFLAGS = -module -avoid-version
# verilog_la_CPPFLAGS = $(AM_CPPFLAGS)
# need Qt for TextStream (BUG?)
verilog_la_CPPFLAGS = $(AM_CPPFLAGS) $(QT_INCLUDES)

qucsator_la_SOURCES = qucsator.cpp
qucsator_la_LDFLAGS = -module -avoid-version
# qucsator_la_CPPFLAGS = $(AM_CPPFLAGS)
# need Qt for TextStream. (BUG?)
qucsator_la_CPPFLAGS = $(AM_CPPFLAGS) $(QT_INCLUDES)
