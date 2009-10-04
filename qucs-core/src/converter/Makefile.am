## Process this file with automake to produce Makefile.in
# 
# src/converter/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

EXTRA_DIST = parse_spice.y scan_spice.l parse_vcd.y scan_vcd.l

bin_PROGRAMS = qucsconv

qucsconv_LDADD = ../libqucsator.a ../math/libqucsmath.a

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/math

qucsconv_SOURCES = qucsconv.cpp parse_spice.cpp scan_spice.cpp \
	check_spice.cpp qucs_producer.cpp parse_vcd.cpp scan_vcd.cpp \
	check_vcd.cpp csv_producer.cpp touchstone_producer.cpp \
	matlab_producer.cpp

noinst_HEADERS = tokens_spice.h check_spice.h qucs_producer.h check_vcd.h \
	tokens_vcd.h csv_producer.h touchstone_producer.h matlab_producer.h

if MAINTAINER_MODE
generated_FILES = parse_spice.cpp scan_spice.cpp tokens_spice.h \
	parse_vcd.cpp scan_vcd.cpp tokens_vcd.h

scan_spice.cpp: scan_spice.l
	$(LEX) -o$@ $<

parse_spice.cpp: parse_spice.y
	$(YACC) -v -o $@ --defines=tokens_spice.h $<

scan_vcd.cpp: scan_vcd.l
	$(LEX) -o$@ $<

parse_vcd.cpp: parse_vcd.y
	$(YACC) -v -o $@ --defines=tokens_vcd.h $<

else
generated_FILES = 
endif

CLEANFILES = *~ *.orig *.rej *.output
MAINTAINERCLEANFILES = Makefile.in $(generated_FILES)
DISTCLEANFILES = $(generated_FILES)
