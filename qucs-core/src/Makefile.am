## Process this file with automake to produce Makefile.in
# 
# src/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.  
#

EXTRA_DIST = parse_netlist.y scan_netlist.l scan_dataset.l parse_dataset.y \
	parse_touchstone.y scan_touchstone.l

SUBDIRS = components

bin_PROGRAMS = qucsator

INCLUDES = -I$(top_srcdir)/src/components

qucsator_LDADD = components/libcomponent.a

qucsator_SOURCES = ucs.cpp logging.c strlist.cpp complex.cpp circuit.cpp    \
	check_netlist.c parse_netlist.c scan_netlist.c dataset.cpp node.cpp \
	check_dataset.cpp parse_dataset.cpp scan_dataset.cpp property.cpp   \
	vector.cpp object.cpp net.cpp input.cpp check_touchstone.cpp        \
	parse_touchstone.cpp scan_touchstone.cpp matrix.cpp analysis.cpp    \
	spsolver.cpp dcsolver.cpp nodelist.cpp

noinst_HEADERS = check_netlist.h complex.h input.h net.h object.h circuit.h \
	tokens_netlist.h dataset.h logging.h node.h property.h vector.h     \
	tokens_dataset.h check_dataset.h strlist.h tokens_touchstone.h      \
	check_touchstone.h matrix.h analysis.h spsolver.h dcsolver.h        \
	nodelist.h


generated_FILES = scan_netlist.c parse_netlist.c scan_dataset.cpp \
	parse_dataset.cpp tokens_dataset.h tokens_netlist.h       \
	parse_touchstone.cpp tokens_touchstone.h scan_touchstone.cpp

scan_netlist.c: scan_netlist.l
	$(LEX) -o$@ $<

scan_dataset.cpp: scan_dataset.l
	$(LEX) -o$@ $<

scan_touchstone.cpp: scan_touchstone.l
	$(LEX) -o$@ $<

parse_netlist.c: parse_netlist.y
	$(YACC) -v -o $@ --defines=tokens_netlist.h $<

parse_dataset.cpp: parse_dataset.y
	$(YACC) -v -o $@ --defines=tokens_dataset.h $<

parse_touchstone.cpp: parse_touchstone.y
	$(YACC) -v -o $@ --defines=tokens_touchstone.h $<

CLEANFILES = *~ *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in $(generated_FILES)
DISTCLEANFILES = $(generated_FILES)
