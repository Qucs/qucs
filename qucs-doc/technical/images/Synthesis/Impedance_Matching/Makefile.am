## Process this file with automake to produce Makefile.in
# 
# tutorial/equations/pics/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>
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


EXTRA_DIST = $(EPS_MATCHING) 

EPS_MATCHING = CascadedLCsch-highpass.eps              Double-Tapped-Resonator-Transformed.eps  pi-type-input-transform.eps                  Smithl8l4.eps \
CascadedLCsch-lowpass.eps               Highpass-Pi.eps                          pi-type-network.eps                          Tapped-C-ckt-equivalent.eps \
cascaded-l-section-highpass.eps         Highpass-Tee.eps                         pi-type-output-transform.eps                 Tapped-C-ckt-equivalent-input-pair-transformed.eps \
cascaded-l-section-lowpass.eps          Lowpass-Pi.eps                           series-double-tuned-transformer.eps          Tapped-C-Transformer.eps \
coupled-inductors.eps                   Lowpass-Tee.eps                          Series_to_parallel.eps                       Tapped-L.eps \
coupled-inductors-pi-equivalent.eps     Lsection.eps                             SingleStubOpenBalanced.eps                   Tapped-L-equivalent.eps \
coupled-inductors-tee-equivalent.eps   SingleStubOpen.eps                 Tapped-L-transformation-input.eps \
DoubleStubOpenBalanced.eps                                        SingleStubShortBalanced.eps                  Tapped-L-transformation-output.eps \
DoubleStubOpen.eps                                             SingleStubShort.eps                          Tee-to-pi-transformation.eps \
DoubleStubShortBalanced.eps             parallel-double-tuned-transformer.eps    Single-tuned-transformer.eps                 TL-circuit-equivalent-HP-Pi-Tee.eps \
DoubleStubShort.eps                             Parallel_to_series.eps                   Single-tuned-transformer-equivalent-ckt.eps  TL-circuit-equivalent-LP-Pi-Tee.eps \
Double-Tapped-Resonator.eps             pi-equivalent-circuit.eps                SmithCascadedLCsections.eps \
Double-Tapped-Resonator-p2s-input.eps   pi-to-tee-equivalent-implementation.eps  SmithChart-Lsection.eps \
Double-Tapped-Resonator-p2s-output.eps  Pi-to-tee-transformation.eps             SmithChartTL.eps

SUFFIXES = .fig .eps

.fig.eps: $(EPS_MATCHING) 
	$(FIG2DEV) -L eps $< > $@

CLEANFILES = *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
