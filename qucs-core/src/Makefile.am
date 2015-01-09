## Process this file with automake to produce Makefile.in
#
# src/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2003-2009 Stefan Jahn <stefan@lkcc.org>
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

EXTRA_DIST = parse_netlist.y scan_netlist.l scan_dataset.l parse_dataset.y \
	parse_touchstone.y scan_touchstone.l parse_citi.y scan_citi.l \
	gperfapphash.cpp parse_zvr.y scan_zvr.l parse_mdl.y scan_mdl.l \
	parse_csv.y scan_csv.l

SUBDIRS = math components interface . converter

AM_CPPFLAGS = -I$(top_srcdir)/src/math -I$(top_srcdir)/src/components \
	-I$(top_srcdir)/src/interface -I$(builddir)

lib_LTLIBRARIES = libqucs.la

libqucs_la_LIBADD = math/libqucsmath.la   \
	components/libcomponent.la	      \
	components/microstrip/libmicrostrip.la \
	components/devices/libdevices.la	      \
	components/digital/libdigital.la       \
	components/verilog/libverilog.la \
	interface/libinterface.la
## In case we want to build windows dlls
libqucs_la_LDFLAGS = -shared -no-undefined

pkginclude_HEADERS = compat.h logging.h object.h vector.h consts.h node.h \
  net.h circuit.h integrator.h states.h states.cpp valuelist.h \
  constants.h netdefs.h

noinst_TEMPLATES = tridiag.cpp hash.cpp \
	tmatrix.cpp tvector.cpp eqnsys.cpp states.cpp \
	nasolver.cpp

noinst_HEADERS = $(noinst_TEMPLATES)            \
	tokens_netlist.h tokens_dataset.h check_dataset.h tokens_touchstone.h       \
	check_touchstone.h  spsolver.h dcsolver.h property.h variable.h       \
	parasweep.h sweep.h libqucsator.h evaluate.h matvec.h acsolver.h   \
	transient.h netdefs.h qucsdefs.h ptrlist.h hbsolver.h poly.h     \
	spline.h tridiag.h fourier.h hash.h applications.h characteristic.h    \
	range.h history.h devstates.h tokens_citi.h check_citi.h check_zvr.h  \
	tokens_zvr.h check_mdl.h tokens_mdl.h differentiate.h tokens_csv.h  \
	check_csv.h analyses.h receiver.h interpolator.h
	logging.h net.h input.h dataset.h equation.h tvector.h tmatrix.h \
	environment.h exceptionstack.h check_netlist.h module.h nasolver.h \
	states.h analysis.h trsolver.h nasolution.h eqnsys.h compat.h \
	exception.h object.h node.h circuit.h constants.h vector.h \
	nodeset.h nodelist.h strlist.h operatingpoint.h pair.h consts.h  \
	integrator.h valuelist.h

libqucs_la_SOURCES = dataset.cpp check_dataset.cpp parse_dataset.cpp      \
	scan_dataset.cpp check_touchstone.cpp vector.cpp object.cpp          \
	property.cpp parse_touchstone.cpp scan_touchstone.cpp variable.cpp   \
	strlist.cpp logging.c exception.cpp exceptionstack.cpp               \
	matvec.cpp parse_citi.cpp                                            \
	scan_citi.cpp check_citi.cpp parse_zvr.cpp scan_zvr.cpp sweep.cpp    \
	check_zvr.cpp parse_mdl.cpp scan_mdl.cpp check_mdl.cpp check_csv.cpp \
	parse_csv.cpp scan_csv.cpp circuit.cpp check_netlist.cpp \
	parse_netlist.cpp scan_netlist.cpp net.cpp input.cpp        \
	analysis.cpp spsolver.cpp dcsolver.cpp nodelist.cpp environment.cpp  \
	parasweep.cpp equation.cpp evaluate.cpp acsolver.cpp                 \
	trsolver.cpp transient.cpp integrator.cpp nodeset.cpp hbsolver.cpp   \
	spline.cpp fourier.cpp history.cpp       \
	range.cpp devstates.cpp differentiate.cpp module.cpp receiver.cpp    \
	interpolator.cpp

bin_PROGRAMS = qucsator

qucsator_LDADD = libqucs.la

qucsator_LDFLAGS = -Wl,-rpath,$(libdir)

qucsator_SOURCES = ucs.cpp

if MAINTAINER_MODE
## Here we create the various input parsing code files using
## lex and yacc, these files will be distributed
generated_FILES = scan_netlist.cpp parse_netlist.cpp scan_dataset.cpp     \
	parse_dataset.cpp tokens_dataset.h tokens_netlist.h               \
	parse_touchstone.cpp tokens_touchstone.h scan_touchstone.cpp      \
	parse_citi.cpp tokens_citi.h scan_citi.cpp parse_zvr.cpp          \
	scan_zvr.cpp tokens_zvr.h parse_mdl.cpp	scan_mdl.cpp tokens_mdl.h \
	parse_csv.cpp tokens_csv.h scan_csv.cpp qucsdefs.h

scan_netlist.cpp: scan_netlist.l
	$(LEX) -o$@ $<

scan_dataset.cpp: scan_dataset.l
	$(LEX) -o$@ $<

scan_touchstone.cpp: scan_touchstone.l
	$(LEX) -o$@ $<

scan_csv.cpp: scan_csv.l
	$(LEX) -o$@ $<

scan_citi.cpp: scan_citi.l
	$(LEX) -o$@ $<

scan_zvr.cpp: scan_zvr.l
	$(LEX) -o$@ $<

scan_mdl.cpp: scan_mdl.l
	$(LEX) -o$@ $<

parse_netlist.cpp: parse_netlist.y
	$(YACC) -v -o $@ --defines=tokens_netlist.h $<

parse_dataset.cpp: parse_dataset.y
	$(YACC) -v -o $@ --defines=tokens_dataset.h $<

parse_touchstone.cpp: parse_touchstone.y
	$(YACC) -v -o $@ --defines=tokens_touchstone.h $<

parse_csv.cpp: parse_csv.y
	$(YACC) -v -o $@ --defines=tokens_csv.h $<

parse_citi.cpp: parse_citi.y
	$(YACC) -v -o $@ --defines=tokens_citi.h $<

parse_zvr.cpp: parse_zvr.y
	$(YACC) -v -o $@ --defines=tokens_zvr.h $<

parse_mdl.cpp: parse_mdl.y
	$(YACC) -v -o $@ --defines=tokens_mdl.h $<
else
generated_FILES =
endif

if MAINTAINER_MODE
qucsdefs.h: qucsator$(EXEEXT)
	./$< -l > $@
endif

## The qucs equation definitions use a hash lookup table
## generated using gperf and put in gperfapphash.cpp which
## is #included in the body of equation.cpp
equation.cpp: gperfapphash.cpp

if MAINTAINER_MODE
## Here we generate gperfapphash.cpp for equation.cpp.
##
## To generate the hash table code we must first create a
## program to create the hash input file for gperf, called
## gperfappgen.
noinst_PROGRAMS = gperfappgen

gperfappgen_SOURCES = gperfappgen.cpp

gperfapphash.gph: gperfappgen$(EXEEXT)
	./$< > $@

gperfapphash.cpp: gperfapphash.gph
	$(GPERF) -I -m 8 $< | sed -e 's/{""},/{"",0},/g' > $@

gperfappgen.cpp: gperfappgen.h

gperfappgen.h: applications.h
	sed -e 's/evaluate::[a-zA-Z0-9_]*/NULL/g' < $< > $@

## Store the gperf names in their own variable as the are
## not distributed but must be removed in a maintainer-clean
gperf_FILES = gperfapphash.cpp gperfapphash.gph gperfappgen.h

else
gperf_FILES =
endif

CLEANFILES = *~ *.orig *.rej *.output
MAINTAINERCLEANFILES = Makefile.in $(generated_FILES) $(gperf_FILES)
DISTCLEANFILES = $(generated_FILES)
