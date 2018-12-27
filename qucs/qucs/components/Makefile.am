## Process this file with automake to produce Makefile.in
#
# qucs/components/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
# New models 2013 addded Mike Brinson  mbrin72043@yahoo.co.uk
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

# private library
qucsdir=${pkglibdir}

qucs_LTLIBRARIES = libcomponents.la

MOCHEADERS =
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

plugindir = ${pkglibdir}
plugin_LTLIBRARIES = qucs-default-components.la

qucs_default_components_la_LDFLAGS = -shared -module -avoid-version
qucs_default_components_la_SOURCES = \
	resistor.cpp \
	spicedialog.cpp \
	volt_dc.cpp
# TODO: move the others
LATER= \
  volt_ac.cpp subcircuit.cpp sparamfile.cpp            \
  source_ac.cpp isolator.cpp circulator.cpp attenuator.cpp       \
  ampere_dc.cpp transformer.cpp symtrafo.cpp \
  mutual2.cpp noise_ii.cpp noise_iv.cpp noise_vv.cpp logical_or.cpp           \
  logical_nor.cpp logical_and.cpp logical_nand.cpp logical_xor.cpp            \
  logical_xnor.cpp logical_inv.cpp digi_source.cpp coupler.cpp   \
  rs_flipflop.cpp d_flipflop.cpp jk_flipflop.cpp coaxialline.cpp vprobe.cpp   \
  switch.cpp relais.cpp am_modulator.cpp pm_modulator.cpp        \
  tline_4port.cpp twistedpair.cpp bondwire.cpp rlcg.cpp    \
  vexp.cpp iexp.cpp eqndefined.cpp  \
  vfile.cpp ifile.cpp mod_amp.cpp thyristor.cpp triac.cpp       \
  diac.cpp log_amp.cpp potentiometer.cpp rfedd.cpp rfedd2p.cpp  \
  MESFET.cpp rectline.cpp photodiode.cpp     \
  EKV26MOS.cpp logical_buf.cpp dff_SR.cpp jkff_SR.cpp tff_SR.cpp              \
  gatedDlatch.cpp logic_0.cpp logic_1.cpp mux2to1.cpp mux4to1.cpp mux8to1.cpp \
  DLS_nto1.cpp DLS_1ton.cpp andor4x2.cpp andor4x3.cpp andor4x4.cpp            \
  dmux2to4.cpp dmux3to8.cpp dmux4to16.cpp ha1b.cpp fa1b.cpp fa2b.cpp         

# simulation commands.
# these are not pluggable. compile them in.
SIM_THINGS = \
	dc_sim.cpp tr_sim.cpp sp_sim.cpp hb_sim.cpp opt_sim.cpp \
	ac_sim.cpp digi_sim.cpp param_sweep.cpp

MORE_LATER = phaseshifter.cpp gyrator.cpp \
  vcvs.cpp vccs.cpp tline.cpp substrate.cpp \
  mstee.cpp msstep.cpp msline.cpp mscross.cpp mscoupled.cpp mslange.cpp   \
  mscorner.cpp diode.cpp coplanar.cpp ccvs.cpp cccs.cpp \
  dcfeed.cpp dcblock.cpp biast.cpp inductor.cpp capacitor.cpp \
  iprobe.cpp volt_noise.cpp ampere_noise.cpp       \
  msmbend.cpp msopen.cpp ampere_ac.cpp bjt.cpp jfet.cpp mosfet.cpp            \
  msgap.cpp bjtsub.cpp mosfet_sub.cpp vpulse.cpp ipulse.cpp vrect.cpp         \
  irect.cpp msvia.cpp amplifier.cpp opamp.cpp cpwopen.cpp       \
  cpwshort.cpp cpwgap.cpp cpwstep.cpp mutual.cpp  

# are these really *source* files?!
# verilogstuff =
# binarytogrey4bit.cpp        \
#  pad2bit.cpp pad3bit.cpp pad4bit.cpp
#   greytobinary4bit.cpp comp_1bit.cpp comp_2bit.cpp comp_4bit.cpp              \
#   hpribin4bit.cpp msrstub.cpp phototransistor.cpp nigbt.cpp \
#   hybrid.cpp ctline.cpp tunneldiode.cpp \
#   etr_sim.cpp ecvs.cpp vcresistor.cpp       \
#   mutualx.cpp

# some stuff mostly related to legacy components
RANDOM_LEGACY_STUFF = \
	vacomponent.cpp \
	vhdlfile.cpp libcomp.cpp ground.cpp \
	verilogfile.cpp \
  	subcirport.cpp \
	equation.cpp \
	vafile.cpp \
	spicefile.cpp

libcomponents_la_SOURCES = $(MOCFILES) $(SIM_THINGS) \
	$(RANDOM_LEGACY_STUFF) \
	$(DIALG_SRC)

noinst_HEADERS = $(MOCHEADERS) components.h capacitor.h vccs.h     \
	spicedialog.h optimizedialog.h spicefile.h \
	componentdialog.h \
  inductor.h biast.h dcblock.h dcfeed.h ground.h subcirport.h symtrafo.h      \
  transformer.h ampere_dc.h attenuator.h circulator.h equation.h isolator.h   \
  source_ac.h subcircuit.h volt_ac.h ac_sim.h cccs.h   \
  ccvs.h coplanar.h dc_sim.h diode.h hb_sim.h mscorner.h mscoupled.h mslange.h          \
  mscross.h msline.h msstep.h mstee.h param_sweep.h sp_sim.h substrate.h      \
  tline.h tr_sim.h component.h vcvs.h gyrator.h phaseshifter.h vhdlfile.h     \
  iprobe.h wprobe.h volt_noise.h ampere_noise.h msmbend.h msopen.h ampere_ac.h bjt.h   \
  jfet.h mosfet.h msgap.h bjtsub.h mosfet_sub.h vpulse.h ipulse.h vrect.h     \
  irect.h msvia.h amplifier.h opamp.h spicefile.h cpwopen.h cpwshort.h        \
  cpwgap.h cpwstep.h libcomp.h mutual.h mutual2.h noise_ii.h noise_iv.h       \
  noise_vv.h logical_or.h logical_nor.h logical_and.h logical_nand.h          \
  logical_xor.h logical_xnor.h logical_inv.h digi_sim.h digi_source.h         \
  coupler.h rs_flipflop.h d_flipflop.h jk_flipflop.h coaxialline.h vprobe.h   \
  switch.h relais.h am_modulator.h pm_modulator.h opt_sim.h tline_4port.h     \
  twistedpair.h bondwire.h verilogfile.h vexp.h iexp.h  \
  eqndefined.h vfile.h ifile.h mod_amp.h thyristor.h triac.h      \
  diac.h log_amp.h potentiometer.h rfedd.h rfedd2p.h MESFET.h     \
  rectline.h photodiode.h EKV26MOS.h rlcg.h      \
  logical_buf.h dff_SR.h jkff_SR.h tff_SR.h gatedDlatch.h logic_0.h logic_1.h \
  mux2to1.h mux4to1.h mux8to1.h DLS_nto1.h DLS_1ton.h andor4x2.h andor4x3.h   \
  andor4x4.h dmux2to4.h dmux3to8.h dmux4to16.h ha1b.h fa1b.h fa2b.h pad2bit.h \
  pad3bit.h pad4bit.h binarytogrey4bit.h greytobinary4bit.h comp_1bit.h       \
  comp_2bit.h comp_4bit.h hpribin4bit.h msrstub.h phototransistor.h nigbt.h   \
  vafile.h hybrid.h ctline.h tunneldiode.h      \
  etr_sim.h ecvs.h vcresistor.h vacomponent.h   \
  mutualx.h circline.h taperedline.h \
  capq.h indq.h spiralinductor.h circularloop.h spdeembed.h

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS) -I$(top_srcdir)/qucs

SUFFIXES = .moc.cpp

.h.moc.cpp:
	$(MOC) -o $@ $<

CLEANFILES = *~ *.moc.cpp
MAINTAINERCLEANFILES = Makefile.in
