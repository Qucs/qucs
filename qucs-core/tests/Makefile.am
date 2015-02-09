## Process this file with automake to produce Makefile.in
#
# Makefile.am
#
# Automake input file.
#
# Copyright (C) 2014 Guilherme Brondani Torri <guitorri@gmail.com>
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

AM_CPPFLAGS =  \
    -I$(top_srcdir)/libs/gtest/include \
    -I$(top_srcdir)/libs/gtest/ \
    -I$(top_srcdir)/src \
    -I$(top_srcdir)/src/math \
    -I$(top_srcdir)/src/components

AM_CXXFLAGS = $(AM_CPPFLAGS)

AM_LDFLAGS = \
	 $(top_srcdir)/src/libqucs.la


# TESTS -- Programs run automatically by "make check"
TESTS =
check_PROGRAMS =

# Test libqucs
TESTS += libqucsUnitTest
check_PROGRAMS += libqucsUnitTest
libqucsUnitTest_SOURCES = testMain.cpp \
  test_libqucs.cpp \
	Fourier.cpp \
	Math.cpp \
	Matrix.cpp \
	Vector.cpp

libqucsUnitTest_CXXFLAGS = "-DGTEST_HAS_PTHREAD=0"

