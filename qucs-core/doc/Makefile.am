## Process this file with automake to produce Makefile.in
# 
# doc/Makefile.am
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

EXTRA_DIST = technical.tex $(srcdir)/technical.ps $(FIGURES) $(BIBFILE)

CLEANFILES = *~ $(EPSFILES) *.bak *.aux *.log *.toc *.pdf *.bbl *.blg *.dvi
MAINTAINERCLEANFILES = Makefile.in
DISTCLEANFILES = *.ps

LATEX = latex
BIBTEX = bibtex
PDFLATEX = pdflatex
DVIPS = dvips
PS2EPS = ps2eps
EPSTOPDF = epstopdf
FIG2DEV = fig2dev

FIGURES = differential.fig symtrafo.fig biast.fig trafo.fig xCCS.fig xCVS.fig \
  nconnect.fig Sparam.fig cccs.fig vccs.fig vcvs.fig ccvs.fig acstrafo.fig    \
  actrafo.fig
EPSFILES = $(FIGURES:.fig=.eps)
PDFFILES = $(FIGURES:.fig=.pdf)
BIBFILE = $(srcdir)/literature.bib

technical.dvi: $(EPSFILES)

%.dvi: %.tex $(BIBFILE)
	$(LATEX) $<
	$(BIBTEX) $*
	$(LATEX) $<
	$(LATEX) -interaction=batchmode $<

%.pdf: %.tex $(PDFFILES)
	$(PDFLATEX) $<
	$(BIBTEX) $*
	$(PDFLATEX) $<
	$(PDFLATEX) -interaction=batchmode $<

%.eps: %.fig
	$(FIG2DEV) -L eps $< > $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< > $@

%.ps: %.dvi
	$(DVIPS) $< -o $@
