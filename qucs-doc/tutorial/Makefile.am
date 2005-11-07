## Process this file with automake to produce Makefile.in
# 
# tutorial/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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

SUBDIRS = dcstatic coupler bjtbias modelbjt poweramp lna

EXTRA_DIST = $(SOURCES) $(TEXFILES)

TEXFILES = titlepage.tex basics.tex workbook.tex intro.tex general.tex \
  datasheet.tex
TARGETS = workbook.pdf
PICDIRS = coupler

pics-book:
	@for d in $(PICDIRS); do \
	  (cd $$d && $(MAKE) $(AM_MAKEFLAGS) book); \
	done

book: pics-book $(TARGETS)

tutorial:
	@for d in $(SUBDIRS); do \
	  (cd $$d && $(MAKE) $(AM_MAKEFLAGS) tutorial); \
	done

%.dvi: %.tex
	$(LATEX) $<
	$(LATEX) $<
	$(LATEX) -interaction=batchmode $<

%.pdf: %.tex
	$(PDFLATEX) $<
	$(PDFLATEX) $<
	$(PDFLATEX) -interaction=batchmode $<

%.eps: %.fig
	$(FIG2DEV) -L eps $< > $@

%.pdf: %.eps
	$(EPSTOPDF) $< -o=$@

%.ps: %.dvi
	$(DVIPS) $< -o $@

CLEANFILES = *~ *.bak *.aux *.log *.toc *.pdf *.bbl *.blg *.dvi *.brf *.out
MAINTAINERCLEANFILES = Makefile.in
DISTCLEANFILES = *.ps *.pdf
