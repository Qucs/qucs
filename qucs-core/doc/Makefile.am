## Process this file with automake to produce Makefile.in
# 
# doc/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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

EXTRA_DIST = technical.tex $(srcdir)/technical.ps $(FIGURES) $(BIBFILE) \
  $(PSFILES) $(MATHFILES) PLATFORMS $(MATHHELPS) $(CHAPTERS)

dist_man_MANS = qucsator.1

CLEANFILES = *~ $(EPSFILES) *.bak *.aux *.log *.toc *.pdf *.bbl *.blg *.dvi \
  $(MPSFILES) $(PDFFIGURES) $(PDFFILES)
MAINTAINERCLEANFILES = Makefile.in
DISTCLEANFILES = *.ps *.pdf

LATEX = latex
BIBTEX = bibtex
PDFLATEX = pdflatex
DVIPS = dvips
PS2EPS = ps2eps
EPSTOPDF = epstopdf
FIG2DEV = fig2dev
OCTAVE = octave

MATHFILES = mszl.m mscomparezl.m radius.m stabgear.m stabmoulton.m \
  stabbashford.m stabgearex.m
MATHHELPS = zpoly.m econst.m
MPSFILES = $(MATHFILES:.m=.eps)
FIGURES = differential.fig symtrafo.fig biast.fig trafo.fig xCCS.fig xCVS.fig \
  nconnect.fig Sparam.fig cccs.fig vccs.fig vcvs.fig ccvs.fig acstrafo.fig    \
  actrafo.fig opamp.fig gyrator.fig msline.fig newton.fig jfet.fig dcjfet.fig \
  newtondiode.fig dcdiode.fig diode.fig spdiode.fig spjfet.fig newtonbad.fig  \
  sconnectgraph.fig sconnectloop.fig sconnectpath.fig siconnectgraph.fig      \
  siconnectpath.fig siconnectloop.fig twoportiv.fig twoportab.fig dcsgp.fig   \
  twoportss.fig twoportpp.fig twoportps.fig twoportsp.fig twoportch.fig       \
  nconnect2.fig nconnect3.fig mscorner.fig sgp.fig dcsgp_spice.fig spsgp.fig  \
  noisejfet.fig noisediode.fig noisesgp.fig tcircuit.fig bridgecircuit.fig    \
  xcircuit.fig picircuit.fig mscoupled.fig msgap.fig mosphysical.fig          \
  mostypes.fig mosfet.fig mosband.fig dcmosfet.fig spmosfet.fig msgapphys.fig \
  noisemosfet.fig mscoupledphys.fig spfile.fig spnfile.fig transcap.fig       \
  feuler.fig beuler.fig trapez.fig transind.fig stiffstable.fig convprob1.fig \
  convprob2.fig convprob3.fig
PSFILES = MNAexample.eps NLexample.eps CYexample.eps
EPSFILES = $(FIGURES:.fig=.eps)
PDFFILES = $(FIGURES:.fig=.pdf)
PDFFIGURES = $(PSFILES:.eps=.pdf) $(MPSFILES:.eps=.pdf)
BIBFILE = $(srcdir)/literature.bib
CHAPTERS = sparameter.tex noisewaves.tex dcanalysis.tex acanalysis.tex \
  transient.tex devices.tex microstrip.tex

technical.dvi: $(EPSFILES) $(PSFILES) $(MPSFILES)

%.dvi: %.tex $(CHAPTERS) $(BIBFILE)
	$(LATEX) $<
	$(BIBTEX) $*
	$(LATEX) $<
	$(LATEX) -interaction=batchmode $<

%.pdf: %.tex $(PDFFILES) $(PDFFIGURES)
	$(PDFLATEX) $<
	$(BIBTEX) $*
	$(PDFLATEX) $<
	$(PDFLATEX) -interaction=batchmode $<

%.eps: %.fig
	$(FIG2DEV) -L eps $< > $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< > $@

%.pdf: %.eps
	$(EPSTOPDF) $< -o=$@

%.eps: %.m
	$(OCTAVE) $<

%.ps: %.dvi
	$(DVIPS) $< -o $@
