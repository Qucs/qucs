## Process this file with automake to produce Makefile.in
# 
# qucs/components/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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

noinst_LIBRARIES = libcomponents.a

MOCHEADERS = componentdialog.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

libcomponents_a_SOURCES = phaseshifter.cpp gyrator.cpp componentdialog.cpp    \
  vcvs.cpp vccs.cpp tr_sim.cpp tline.cpp substrate.cpp sp_sim.cpp             \
  param_sweep.cpp mstee.cpp msstep.cpp msline.cpp mscross.cpp mscoupled.cpp   \
  mscorner.cpp hb_sim.cpp diode.cpp dc_sim.cpp coplanar.cpp ccvs.cpp cccs.cpp \
  ac_sim.cpp volt_dc.cpp volt_ac.cpp subcircuit.cpp sparamfile.cpp            \
  source_ac.cpp isolator.cpp equation.cpp circulator.cpp attenuator.cpp       \
  ampere_dc.cpp transformer.cpp symtrafo.cpp subcirport.cpp ground.cpp        \
  dcfeed.cpp dcblock.cpp biast.cpp inductor.cpp capacitor.cpp resistorus.cpp  \
  component.cpp resistor.cpp $(MOCFILES)

noinst_HEADERS = $(MOCHEADERS) resistor.h components.h capacitor.h vccs.h     \
  inductor.h biast.h dcblock.h dcfeed.h ground.h subcirport.h symtrafo.h      \
  transformer.h ampere_dc.h attenuator.h circulator.h equation.h isolator.h   \
  source_ac.h sparamfile.h subcircuit.h volt_ac.h volt_dc.h ac_sim.h cccs.h   \
  ccvs.h coplanar.h dc_sim.h diode.h hb_sim.h mscorner.h mscoupled.h          \
  mscross.h msline.h msstep.h mstee.h param_sweep.h sp_sim.h substrate.h      \
  tline.h tr_sim.h component.h vcvs.h gyrator.h phaseshifter.h resistorus.h

INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES)

%.moc.cpp: %.h
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
