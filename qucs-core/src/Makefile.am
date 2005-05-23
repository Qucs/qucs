## Process this file with automake to produce Makefile.in
# 
# src/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2003, 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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

SUBDIRS = converter components

bin_PROGRAMS = qucsator

INCLUDES = -I$(top_srcdir)/src/components

qucsator_LDADD = libqucsator.a \
	components/libcomponent.a \
	components/microstrip/libmicrostrip.a \
	components/devices/libdevices.a

qucsator_SOURCES = ucs.cpp circuit.cpp check_netlist.cpp operatingpoint.cpp  \
	parse_netlist.cpp scan_netlist.cpp node.cpp net.cpp input.cpp        \
	analysis.cpp spsolver.cpp dcsolver.cpp nodelist.cpp environment.cpp  \
	parasweep.cpp equation.cpp evaluate.cpp sweep.cpp acsolver.cpp       \
	trsolver.cpp transient.cpp integrator.cpp nodeset.cpp hbsolver.cpp   \
	spline.cpp

noinst_TEMPLATES = tmatrix.cpp eqnsys.cpp nasolver.cpp states.cpp tvector.cpp \
	ptrlist.cpp

noinst_HEADERS = $(noinst_TEMPLATES) check_netlist.h complex.h input.h net.h  \
	tokens_netlist.h dataset.h logging.h node.h property.h vector.h       \
	tokens_dataset.h check_dataset.h strlist.h tokens_touchstone.h        \
	check_touchstone.h matrix.h analysis.h spsolver.h dcsolver.h object.h \
	nodelist.h variable.h environment.h parasweep.h eqnsys.h sweep.h      \
	operatingpoint.h libqucsator.h equation.h evaluate.h exception.h      \
	exceptionstack.h matvec.h constants.h consts.h nasolver.h tmatrix.h   \
	circuit.h acsolver.h trsolver.h transient.h integrator.h states.h     \
	tvector.h nodeset.h netdefs.h qucsdefs.h ptrlist.h hbsolver.h poly.h  \
	spline.h

noinst_LIBRARIES = libqucsator.a

libqucsator_a_SOURCES = dataset.cpp check_dataset.cpp parse_dataset.cpp     \
	scan_dataset.cpp check_touchstone.cpp vector.cpp object.cpp         \
	property.cpp parse_touchstone.cpp scan_touchstone.cpp variable.cpp  \
	complex.cpp strlist.cpp logging.c exception.cpp exceptionstack.cpp  \
	matvec.cpp matrix.cpp

if MAINTAINER_MODE
generated_FILES = scan_netlist.cpp parse_netlist.cpp scan_dataset.cpp \
	parse_dataset.cpp tokens_dataset.h tokens_netlist.h           \
	parse_touchstone.cpp tokens_touchstone.h scan_touchstone.cpp

scan_netlist.cpp: scan_netlist.l
	$(LEX) -o$@ $<

scan_dataset.cpp: scan_dataset.l
	$(LEX) -o$@ $<

scan_touchstone.cpp: scan_touchstone.l
	$(LEX) -o$@ $<

parse_netlist.cpp: parse_netlist.y
	$(YACC) -v -o $@ --defines=tokens_netlist.h $<

parse_dataset.cpp: parse_dataset.y
	$(YACC) -v -o $@ --defines=tokens_dataset.h $<

parse_touchstone.cpp: parse_touchstone.y
	$(YACC) -v -o $@ --defines=tokens_touchstone.h $<
else
generated_FILES = 
endif

CLEANFILES = *~ *.orig *.rej *.output
MAINTAINERCLEANFILES = Makefile.in $(generated_FILES)
DISTCLEANFILES = $(generated_FILES)
