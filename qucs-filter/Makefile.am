## Process this file with automake to produce Makefile.in
#
# qucs-filter/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004, 2006 Stefan Jahn <stefan@lkcc.org>
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
#
MOCHEADERS = qucsfilter.h helpdialog.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

include ../common.mk

bin_PROGRAMS = qucsfilter

dist_man_MANS = qucsfilter.1

qucsfilter_SOURCES =  \
	eqn_filter.cpp \
	filter.cpp \
	helpdialog.cpp \
	lc_filter.cpp \
	line_filter.cpp \
	qf_cauer.cpp \
	qf_filter.cpp \
	qucsfilter.cpp \
	stepz_filter.cpp \
	tl_filter.cpp \
	cline_filter.cpp \
	main.cpp  \
	qf_poly.cpp \
	quarterwave_filter.cpp \
        qw_coupled_ring_filter.cpp \
        ccoupled_shunt_resonators.cpp \
	qucsfilter_.qrc
qucsfilter_CXXFLAGS = ${AM_CXXFLAGS} -fPIC

nodist_qucsfilter_SOURCES = $(MOCFILES)

AM_CPPFLAGS += $(X11_INCLUDES) $(QT_CFLAGS)
qucsfilter_LDFLAGS = $(X11_LDFLAGS) $(QT_LIBS)
qucsfilter_LDADD = $(X11_LIBS) $(QT_LIBS)

noinst_HEADERS = \
  $(MOCHEADERS) \
	eqn_filter.h \
	lc_filter.h \
	qf_poly.h \
	qf_filter.h \
	qf_cauer.h \
	quarterwave_filter.h \
        qw_coupled_ring_filter.h \
        ccoupled_shunt_resonators.h \
	qf_matrix.h \
	line_filter.h \
	filter.h \
	stepz_filter.h \
	tl_filter.h \
	material_props.h \
	cline_filter.h

CLEANFILES = *~ qucsfilter_.cpp
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
EXTRA_DIST = ${BITMAPS}
BITMAPS = \
	bitmaps/Quarterwave_Notch_Microstrip.svg \
	bitmaps/CCoupled_Lines_Microstrip.svg \
	bitmaps/CCoupled_Shunt_Resonators.svg \
	bitmaps/LC_cauer_notch.svg \
	bitmaps/Quarterwave_Bandpass_Microstrip.svg \
	bitmaps/RFEDD.svg \
	bitmaps/LC_Ladder_Tee_HPF.svg \
	bitmaps/Quarterwave_Notch_Ideal.svg \
	bitmaps/LC_Ladder_Pi_BPF.svg \
	bitmaps/Quarterwave_Side_Coupled_Ring.svg \
	bitmaps/LC_Ladder_Pi_Notch.svg \
	bitmaps/LC_Ladder_Pi_HPF.svg \
	bitmaps/LC_Ladder_Tee_LPF.svg \
	bitmaps/Stepped_Impedance_Ideal.svg \
	bitmaps/Stepped_Impedance_Microstrip.svg \
	bitmaps/CCoupled.svg \
	bitmaps/CCoupled_Microstrip.svg \
	bitmaps/LC_Ladder_Tee_Notch.svg \
	bitmaps/big.qucs.xpm \
	bitmaps/LC_cauer_bandpass.svg \
	bitmaps/LC_cauer_lowpass.svg \
	bitmaps/Quarterwave_Bandpass_Ideal.svg \
	bitmaps/LC_Ladder_Pi_LPF.svg \
	bitmaps/LC_Ladder_Tee_BPF.svg \
	bitmaps/CCoupled_Lines.svg
