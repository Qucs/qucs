## Process this file with automake to produce Makefile.in
# 
# src/components/verilog/Makefile.am
#
# Automake input file for the verilog device library.
#
# Copyright (C) 2006, 2007, 2008, 2009 Stefan Jahn <stefan@lkcc.org>
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

# the verilog devices library rules

noinst_LIBRARIES = libverilog.a

libverilog_a_SOURCES = HBT_X.analogfunction.cpp HBT_X.core.cpp \
	hicumL2V2p1.analogfunction.cpp hicumL2V2p1.core.cpp \
	mod_amp.analogfunction.cpp mod_amp.core.cpp \
	hic2_full.analogfunction.cpp hic2_full.core.cpp \
	log_amp.analogfunction.cpp log_amp.core.cpp \
	hic0_full.analogfunction.cpp hic0_full.core.cpp \
	potentiometer.analogfunction.cpp potentiometer.core.cpp \
	MESFET.analogfunction.cpp MESFET.core.cpp \
	EKV26MOS.analogfunction.cpp EKV26MOS.core.cpp \
	hicumL0V1p2.analogfunction.cpp hicumL0V1p2.core.cpp \
	hicumL2V2p23.analogfunction.cpp hicumL2V2p23.core.cpp \
	photodiode.analogfunction.cpp photodiode.core.cpp \
	\
	dff_SR.analogfunction.cpp dff_SR.core.cpp \
	tff_SR.analogfunction.cpp tff_SR.core.cpp \
	jkff_SR.analogfunction.cpp jkff_SR.core.cpp \
	gatedDlatch.analogfunction.cpp gatedDlatch.core.cpp \
	logic_0.analogfunction.cpp logic_0.core.cpp \
	logic_1.analogfunction.cpp logic_1.core.cpp \
	mux2to1.analogfunction.cpp mux2to1.core.cpp \
	mux4to1.analogfunction.cpp mux4to1.core.cpp \
	mux8to1.analogfunction.cpp mux8to1.core.cpp \
	DLS_nto1.analogfunction.cpp DLS_nto1.core.cpp \
	DLS_1ton.analogfunction.cpp DLS_1ton.core.cpp \
	andor4x2.analogfunction.cpp andor4x2.core.cpp \
	andor4x3.analogfunction.cpp andor4x3.core.cpp \
	andor4x4.analogfunction.cpp andor4x4.core.cpp \
	dmux2to4.analogfunction.cpp dmux2to4.core.cpp \
	dmux3to8.analogfunction.cpp dmux3to8.core.cpp \
	dmux4to16.analogfunction.cpp dmux4to16.core.cpp \
	ha1b.analogfunction.cpp ha1b.core.cpp \
	fa1b.analogfunction.cpp fa1b.core.cpp \
	fa2b.analogfunction.cpp fa2b.core.cpp \
	pad2bit.analogfunction.cpp pad2bit.core.cpp \
	pad3bit.analogfunction.cpp pad3bit.core.cpp \
	pad4bit.analogfunction.cpp pad4bit.core.cpp \
	binarytogrey4bit.analogfunction.cpp binarytogrey4bit.core.cpp \
	greytobinary4bit.analogfunction.cpp greytobinary4bit.core.cpp \
	comp_1bit.analogfunction.cpp comp_1bit.core.cpp \
	comp_2bit.analogfunction.cpp comp_2bit.core.cpp \
	comp_4bit.analogfunction.cpp comp_4bit.core.cpp \
	hpribin4bit.analogfunction.cpp hpribin4bit.core.cpp

noinst_HEADERS = HBT_X.analogfunction.h HBT_X.defs.h HBT_X.core.h \
	hicumL2V2p1.analogfunction.h hicumL2V2p1.defs.h hicumL2V2p1.core.h \
	mod_amp.analogfunction.h mod_amp.defs.h mod_amp.core.h \
	hic2_full.analogfunction.h hic2_full.defs.h hic2_full.core.h \
	log_amp.analogfunction.h log_amp.defs.h log_amp.core.h \
	hic0_full.analogfunction.h hic0_full.defs.h hic0_full.core.h \
	potentiometer.analogfunction.h potentiometer.defs.h \
        potentiometer.core.h \
	MESFET.analogfunction.h MESFET.defs.h MESFET.core.h \
	EKV26MOS.analogfunction.h EKV26MOS.defs.h EKV26MOS.core.h \
	hicumL0V1p2.analogfunction.h hicumL0V1p2.defs.h hicumL0V1p2.core.h \
	hicumL2V2p23.analogfunction.h hicumL2V2p23.defs.h hicumL2V2p23.core.h \
	photodiode.analogfunction.h photodiode.defs.h photodiode.core.h \
	\
	dff_SR.analogfunction.h dff_SR.defs.h dff_SR.core.h \
	tff_SR.analogfunction.h tff_SR.defs.h tff_SR.core.h \
	jkff_SR.analogfunction.h jkff_SR.defs.h jkff_SR.core.h \
	gatedDlatch.analogfunction.h gatedDlatch.defs.h gatedDlatch.core.h \
	logic_0.analogfunction.h logic_0.defs.h logic_0.core.h \
	logic_1.analogfunction.h logic_1.defs.h logic_1.core.h \
	mux2to1.analogfunction.h mux2to1.defs.h mux2to1.core.h \
	mux4to1.analogfunction.h mux4to1.defs.h mux4to1.core.h \
	mux8to1.analogfunction.h mux8to1.defs.h mux8to1.core.h \
	DLS_nto1.analogfunction.h DLS_nto1.defs.h DLS_nto1.core.h \
	DLS_1ton.analogfunction.h DLS_1ton.defs.h DLS_1ton.core.h \
	andor4x2.analogfunction.h andor4x2.defs.h andor4x2.core.h \
	andor4x3.analogfunction.h andor4x3.defs.h andor4x3.core.h \
	andor4x4.analogfunction.h andor4x4.defs.h andor4x4.core.h \
	dmux2to4.analogfunction.h dmux2to4.defs.h dmux2to4.core.h \
	dmux3to8.analogfunction.h dmux3to8.defs.h dmux3to8.core.h \
	dmux4to16.analogfunction.h dmux4to16.defs.h dmux4to16.core.h \
	ha1b.analogfunction.h ha1b.defs.h ha1b.core.h \
	fa1b.analogfunction.h fa1b.defs.h fa1b.core.h \
	fa2b.analogfunction.h fa2b.defs.h fa2b.core.h \
	pad2bit.analogfunction.h pad2bit.defs.h pad2bit.core.h \
	pad3bit.analogfunction.h pad3bit.defs.h pad3bit.core.h \
	pad4bit.analogfunction.h pad4bit.defs.h pad4bit.core.h \
	binarytogrey4bit.analogfunction.h binarytogrey4bit.defs.h \
	binarytogrey4bit.core.h	greytobinary4bit.analogfunction.h \
	greytobinary4bit.defs.h greytobinary4bit.core.h \
	comp_1bit.analogfunction.h comp_1bit.defs.h comp_1bit.core.h \
	comp_2bit.analogfunction.h comp_2bit.defs.h comp_2bit.core.h \
	comp_4bit.analogfunction.h comp_4bit.defs.h comp_4bit.core.h \
	hpribin4bit.analogfunction.h hpribin4bit.defs.h hpribin4bit.core.h

VERILOG_FILES = constants.vams disciplines.vams \
  fbh_hbt-2_2a.va hicumL2V2p11.va mod_amp.va hicumL2V2p22.va log_amp.va \
  hicumL0V1p12.va potentiometer.va MESFET.va hicumL0V1p2.va hicumL2V2p23.va \
  photodiode.va EKV26MOS.va \
  \
  dff_SR.va jkff_SR.va tff_SR.va gatedDlatch.va logic_0.va logic_1.va \
  mux2to1.va mux4to1.va mux8to1.va DLS_nto1.va DLS_1ton.va andor4x2.va \
  andor4x3.va andor4x4.va dmux2to4.va dmux3to8.va dmux4to16.va ha1b.va \
  fa1b.va fa2b.va pad2bit.va pad3bit.va pad4bit.va binarytogrey4bit.va \
  greytobinary4bit.va comp_1bit.va comp_2bit.va comp_4bit.va hpribin4bit.va

VERILOGDIGITAL_FILES = 

XML_FILES = analogfunction.xml qucsVersion.xml qucsMODULEcore.xml \
  qucsMODULEdefs.xml qucsMODULEgui.xml

EXTRA_DIST = $(VERILOG_FILES) $(XML_FILES)

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/math \
  -I$(top_srcdir)/src/components -I$(top_srcdir)/src/components/devices

if MAINTAINER_MODE

HBT_X.analogfunction.cpp: analogfunction.xml
HBT_X.analogfunction.cpp: fbh_hbt-2_2a.va
	$(ADMSXML) $< -e analogfunction.xml
HBT_X.core.cpp: HBT_X.defs.h qucsVersion.xml qucsMODULEcore.xml
HBT_X.core.cpp: fbh_hbt-2_2a.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
HBT_X.defs.h: qucsVersion.xml qucsMODULEdefs.xml
HBT_X.defs.h: fbh_hbt-2_2a.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
HBT_X.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
HBT_X.gui.cpp: fbh_hbt-2_2a.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

hicumL2V2p1.analogfunction.cpp: analogfunction.xml
hicumL2V2p1.analogfunction.cpp: hicumL2V2p11.va
	$(ADMSXML) $< -e analogfunction.xml
hicumL2V2p1.core.cpp: hicumL2V2p1.defs.h qucsVersion.xml qucsMODULEcore.xml
hicumL2V2p1.core.cpp: hicumL2V2p11.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
hicumL2V2p1.defs.h: qucsVersion.xml qucsMODULEdefs.xml
hicumL2V2p1.defs.h: hicumL2V2p11.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
hicumL2V2p1.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
hicumL2V2p1.gui.cpp: hicumL2V2p11.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

mod_amp.analogfunction.cpp: analogfunction.xml
mod_amp.analogfunction.cpp: mod_amp.va
	$(ADMSXML) $< -e analogfunction.xml
mod_amp.core.cpp: mod_amp.defs.h qucsVersion.xml qucsMODULEcore.xml
mod_amp.core.cpp: mod_amp.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
mod_amp.defs.h: qucsVersion.xml qucsMODULEdefs.xml
mod_amp.defs.h: mod_amp.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
mod_amp.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
mod_amp.gui.cpp: mod_amp.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

hic2_full.analogfunction.cpp: analogfunction.xml
hic2_full.analogfunction.cpp: hicumL2V2p22.va
	$(ADMSXML) $< -e analogfunction.xml
hic2_full.core.cpp: hic2_full.defs.h qucsVersion.xml qucsMODULEcore.xml
hic2_full.core.cpp: hicumL2V2p22.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
hic2_full.defs.h: qucsVersion.xml qucsMODULEdefs.xml
hic2_full.defs.h: hicumL2V2p22.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
hic2_full.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
hic2_full.gui.cpp: hicumL2V2p22.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

log_amp.analogfunction.cpp: analogfunction.xml
log_amp.analogfunction.cpp: log_amp.va
	$(ADMSXML) $< -e analogfunction.xml
log_amp.core.cpp: log_amp.defs.h qucsVersion.xml qucsMODULEcore.xml
log_amp.core.cpp: log_amp.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
log_amp.defs.h: qucsVersion.xml qucsMODULEdefs.xml
log_amp.defs.h: log_amp.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
log_amp.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
log_amp.gui.cpp: log_amp.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

hic0_full.analogfunction.cpp: analogfunction.xml
hic0_full.analogfunction.cpp: hicumL0V1p12.va
	$(ADMSXML) $< -e analogfunction.xml
hic0_full.core.cpp: hic0_full.defs.h qucsVersion.xml qucsMODULEcore.xml
hic0_full.core.cpp: hicumL0V1p12.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
hic0_full.defs.h: qucsVersion.xml qucsMODULEdefs.xml
hic0_full.defs.h: hicumL0V1p12.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
hic0_full.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
hic0_full.gui.cpp: hicumL0V1p12.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

potentiometer.analogfunction.cpp: analogfunction.xml
potentiometer.analogfunction.cpp: potentiometer.va
	$(ADMSXML) $< -e analogfunction.xml
potentiometer.core.cpp: potentiometer.defs.h qucsVersion.xml qucsMODULEcore.xml
potentiometer.core.cpp: potentiometer.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
potentiometer.defs.h: qucsVersion.xml qucsMODULEdefs.xml
potentiometer.defs.h: potentiometer.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
potentiometer.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
potentiometer.gui.cpp: potentiometer.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

MESFET.analogfunction.cpp: analogfunction.xml
MESFET.analogfunction.cpp: MESFET.va
	$(ADMSXML) $< -e analogfunction.xml
MESFET.core.cpp: MESFET.defs.h qucsVersion.xml qucsMODULEcore.xml
MESFET.core.cpp: MESFET.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
MESFET.defs.h: qucsVersion.xml qucsMODULEdefs.xml
MESFET.defs.h: MESFET.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
MESFET.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
MESFET.gui.cpp: MESFET.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

EKV26MOS.analogfunction.cpp: analogfunction.xml
EKV26MOS.analogfunction.cpp: EKV26MOS.va
	$(ADMSXML) $< -e analogfunction.xml
EKV26MOS.core.cpp: EKV26MOS.defs.h qucsVersion.xml qucsMODULEcore.xml
EKV26MOS.core.cpp: EKV26MOS.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
EKV26MOS.defs.h: qucsVersion.xml qucsMODULEdefs.xml
EKV26MOS.defs.h: EKV26MOS.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
EKV26MOS.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
EKV26MOS.gui.cpp: EKV26MOS.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

hicumL0V1p2.analogfunction.cpp: analogfunction.xml
hicumL0V1p2.analogfunction.cpp: hicumL0V1p2.va
	$(ADMSXML) $< -e analogfunction.xml
hicumL0V1p2.core.cpp: hicumL0V1p2.defs.h qucsVersion.xml qucsMODULEcore.xml
hicumL0V1p2.core.cpp: hicumL0V1p2.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
hicumL0V1p2.defs.h: qucsVersion.xml qucsMODULEdefs.xml
hicumL0V1p2.defs.h: hicumL0V1p2.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
hicumL0V1p2.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
hicumL0V1p2.gui.cpp: hicumL0V1p2.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

hicumL2V2p23.analogfunction.cpp: analogfunction.xml
hicumL2V2p23.analogfunction.cpp: hicumL2V2p23.va
	$(ADMSXML) $< -e analogfunction.xml
hicumL2V2p23.core.cpp: hicumL2V2p23.defs.h qucsVersion.xml qucsMODULEcore.xml
hicumL2V2p23.core.cpp: hicumL2V2p23.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
hicumL2V2p23.defs.h: qucsVersion.xml qucsMODULEdefs.xml
hicumL2V2p23.defs.h: hicumL2V2p23.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
hicumL2V2p23.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
hicumL2V2p23.gui.cpp: hicumL2V2p23.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

photodiode.analogfunction.cpp: analogfunction.xml
photodiode.analogfunction.cpp: photodiode.va
	$(ADMSXML) $< -e analogfunction.xml
photodiode.core.cpp: photodiode.defs.h qucsVersion.xml qucsMODULEcore.xml
photodiode.core.cpp: photodiode.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
photodiode.defs.h: qucsVersion.xml qucsMODULEdefs.xml
photodiode.defs.h: photodiode.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
photodiode.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
photodiode.gui.cpp: photodiode.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

dff_SR.analogfunction.cpp: analogfunction.xml
dff_SR.analogfunction.cpp: dff_SR.va
	$(ADMSXML) $< -e analogfunction.xml
dff_SR.core.cpp: dff_SR.defs.h qucsVersion.xml qucsMODULEcore.xml
dff_SR.core.cpp: dff_SR.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
dff_SR.defs.h: qucsVersion.xml qucsMODULEdefs.xml
dff_SR.defs.h: dff_SR.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
dff_SR.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
dff_SR.gui.cpp: dff_SR.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

tff_SR.analogfunction.cpp: analogfunction.xml
tff_SR.analogfunction.cpp: tff_SR.va
	$(ADMSXML) $< -e analogfunction.xml
tff_SR.core.cpp: tff_SR.defs.h qucsVersion.xml qucsMODULEcore.xml
tff_SR.core.cpp: tff_SR.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
tff_SR.defs.h: qucsVersion.xml qucsMODULEdefs.xml
tff_SR.defs.h: tff_SR.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
tff_SR.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
tff_SR.gui.cpp: tff_SR.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

jkff_SR.analogfunction.cpp: analogfunction.xml
jkff_SR.analogfunction.cpp: jkff_SR.va
	$(ADMSXML) $< -e analogfunction.xml
jkff_SR.core.cpp: jkff_SR.defs.h qucsVersion.xml qucsMODULEcore.xml
jkff_SR.core.cpp: jkff_SR.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
jkff_SR.defs.h: qucsVersion.xml qucsMODULEdefs.xml
jkff_SR.defs.h: jkff_SR.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
jkff_SR.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
jkff_SR.gui.cpp: jkff_SR.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

gatedDlatch.analogfunction.cpp: analogfunction.xml
gatedDlatch.analogfunction.cpp: gatedDlatch.va
	$(ADMSXML) $< -e analogfunction.xml
gatedDlatch.core.cpp: gatedDlatch.defs.h qucsVersion.xml qucsMODULEcore.xml
gatedDlatch.core.cpp: gatedDlatch.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
gatedDlatch.defs.h: qucsVersion.xml qucsMODULEdefs.xml
gatedDlatch.defs.h: gatedDlatch.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
gatedDlatch.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
gatedDlatch.gui.cpp: gatedDlatch.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

logic_0.analogfunction.cpp: analogfunction.xml
logic_0.analogfunction.cpp: logic_0.va
	$(ADMSXML) $< -e analogfunction.xml
logic_0.core.cpp: logic_0.defs.h qucsVersion.xml qucsMODULEcore.xml
logic_0.core.cpp: logic_0.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
logic_0.defs.h: qucsVersion.xml qucsMODULEdefs.xml
logic_0.defs.h: logic_0.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
logic_0.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
logic_0.gui.cpp: logic_0.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

logic_1.analogfunction.cpp: analogfunction.xml
logic_1.analogfunction.cpp: logic_1.va
	$(ADMSXML) $< -e analogfunction.xml
logic_1.core.cpp: logic_1.defs.h qucsVersion.xml qucsMODULEcore.xml
logic_1.core.cpp: logic_1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
logic_1.defs.h: qucsVersion.xml qucsMODULEdefs.xml
logic_1.defs.h: logic_1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
logic_1.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
logic_1.gui.cpp: logic_1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

mux2to1.analogfunction.cpp: analogfunction.xml
mux2to1.analogfunction.cpp: mux2to1.va
	$(ADMSXML) $< -e analogfunction.xml
mux2to1.core.cpp: mux2to1.defs.h qucsVersion.xml qucsMODULEcore.xml
mux2to1.core.cpp: mux2to1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
mux2to1.defs.h: qucsVersion.xml qucsMODULEdefs.xml
mux2to1.defs.h: mux2to1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
mux2to1.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
mux2to1.gui.cpp: mux2to1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

mux4to1.analogfunction.cpp: analogfunction.xml
mux4to1.analogfunction.cpp: mux4to1.va
	$(ADMSXML) $< -e analogfunction.xml
mux4to1.core.cpp: mux4to1.defs.h qucsVersion.xml qucsMODULEcore.xml
mux4to1.core.cpp: mux4to1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
mux4to1.defs.h: qucsVersion.xml qucsMODULEdefs.xml
mux4to1.defs.h: mux4to1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
mux4to1.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
mux4to1.gui.cpp: mux4to1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

mux8to1.analogfunction.cpp: analogfunction.xml
mux8to1.analogfunction.cpp: mux8to1.va
	$(ADMSXML) $< -e analogfunction.xml
mux8to1.core.cpp: mux8to1.defs.h qucsVersion.xml qucsMODULEcore.xml
mux8to1.core.cpp: mux8to1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
mux8to1.defs.h: qucsVersion.xml qucsMODULEdefs.xml
mux8to1.defs.h: mux8to1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
mux8to1.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
mux8to1.gui.cpp: mux8to1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

DLS_nto1.analogfunction.cpp: analogfunction.xml
DLS_nto1.analogfunction.cpp: DLS_nto1.va
	$(ADMSXML) $< -e analogfunction.xml
DLS_nto1.core.cpp: DLS_nto1.defs.h qucsVersion.xml qucsMODULEcore.xml
DLS_nto1.core.cpp: DLS_nto1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
DLS_nto1.defs.h: qucsVersion.xml qucsMODULEdefs.xml
DLS_nto1.defs.h: DLS_nto1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
DLS_nto1.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
DLS_nto1.gui.cpp: DLS_nto1.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

DLS_1ton.analogfunction.cpp: analogfunction.xml
DLS_1ton.analogfunction.cpp: DLS_1ton.va
	$(ADMSXML) $< -e analogfunction.xml
DLS_1ton.core.cpp: DLS_1ton.defs.h qucsVersion.xml qucsMODULEcore.xml
DLS_1ton.core.cpp: DLS_1ton.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
DLS_1ton.defs.h: qucsVersion.xml qucsMODULEdefs.xml
DLS_1ton.defs.h: DLS_1ton.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
DLS_1ton.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
DLS_1ton.gui.cpp: DLS_1ton.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

andor4x2.analogfunction.cpp: analogfunction.xml
andor4x2.analogfunction.cpp: andor4x2.va
	$(ADMSXML) $< -e analogfunction.xml
andor4x2.core.cpp: andor4x2.defs.h qucsVersion.xml qucsMODULEcore.xml
andor4x2.core.cpp: andor4x2.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
andor4x2.defs.h: qucsVersion.xml qucsMODULEdefs.xml
andor4x2.defs.h: andor4x2.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
andor4x2.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
andor4x2.gui.cpp: andor4x2.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

andor4x3.analogfunction.cpp: analogfunction.xml
andor4x3.analogfunction.cpp: andor4x3.va
	$(ADMSXML) $< -e analogfunction.xml
andor4x3.core.cpp: andor4x3.defs.h qucsVersion.xml qucsMODULEcore.xml
andor4x3.core.cpp: andor4x3.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
andor4x3.defs.h: qucsVersion.xml qucsMODULEdefs.xml
andor4x3.defs.h: andor4x3.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
andor4x3.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
andor4x3.gui.cpp: andor4x3.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

andor4x4.analogfunction.cpp: analogfunction.xml
andor4x4.analogfunction.cpp: andor4x4.va
	$(ADMSXML) $< -e analogfunction.xml
andor4x4.core.cpp: andor4x4.defs.h qucsVersion.xml qucsMODULEcore.xml
andor4x4.core.cpp: andor4x4.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
andor4x4.defs.h: qucsVersion.xml qucsMODULEdefs.xml
andor4x4.defs.h: andor4x4.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
andor4x4.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
andor4x4.gui.cpp: andor4x4.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

dmux2to4.analogfunction.cpp: analogfunction.xml
dmux2to4.analogfunction.cpp: dmux2to4.va
	$(ADMSXML) $< -e analogfunction.xml
dmux2to4.core.cpp: dmux2to4.defs.h qucsVersion.xml qucsMODULEcore.xml
dmux2to4.core.cpp: dmux2to4.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
dmux2to4.defs.h: qucsVersion.xml qucsMODULEdefs.xml
dmux2to4.defs.h: dmux2to4.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
dmux2to4.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
dmux2to4.gui.cpp: dmux2to4.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

dmux3to8.analogfunction.cpp: analogfunction.xml
dmux3to8.analogfunction.cpp: dmux3to8.va
	$(ADMSXML) $< -e analogfunction.xml
dmux3to8.core.cpp: dmux3to8.defs.h qucsVersion.xml qucsMODULEcore.xml
dmux3to8.core.cpp: dmux3to8.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
dmux3to8.defs.h: qucsVersion.xml qucsMODULEdefs.xml
dmux3to8.defs.h: dmux3to8.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
dmux3to8.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
dmux3to8.gui.cpp: dmux3to8.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

dmux4to16.analogfunction.cpp: analogfunction.xml
dmux4to16.analogfunction.cpp: dmux4to16.va
	$(ADMSXML) $< -e analogfunction.xml
dmux4to16.core.cpp: dmux4to16.defs.h qucsVersion.xml qucsMODULEcore.xml
dmux4to16.core.cpp: dmux4to16.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
dmux4to16.defs.h: qucsVersion.xml qucsMODULEdefs.xml
dmux4to16.defs.h: dmux4to16.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
dmux4to16.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
dmux4to16.gui.cpp: dmux4to16.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

ha1b.analogfunction.cpp: analogfunction.xml
ha1b.analogfunction.cpp: ha1b.va
	$(ADMSXML) $< -e analogfunction.xml
ha1b.core.cpp: ha1b.defs.h qucsVersion.xml qucsMODULEcore.xml
ha1b.core.cpp: ha1b.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
ha1b.defs.h: qucsVersion.xml qucsMODULEdefs.xml
ha1b.defs.h: ha1b.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
ha1b.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
ha1b.gui.cpp: ha1b.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

fa1b.analogfunction.cpp: analogfunction.xml
fa1b.analogfunction.cpp: fa1b.va
	$(ADMSXML) $< -e analogfunction.xml
fa1b.core.cpp: fa1b.defs.h qucsVersion.xml qucsMODULEcore.xml
fa1b.core.cpp: fa1b.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
fa1b.defs.h: qucsVersion.xml qucsMODULEdefs.xml
fa1b.defs.h: fa1b.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
fa1b.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
fa1b.gui.cpp: fa1b.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

fa2b.analogfunction.cpp: analogfunction.xml
fa2b.analogfunction.cpp: fa2b.va
	$(ADMSXML) $< -e analogfunction.xml
fa2b.core.cpp: fa2b.defs.h qucsVersion.xml qucsMODULEcore.xml
fa2b.core.cpp: fa2b.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
fa2b.defs.h: qucsVersion.xml qucsMODULEdefs.xml
fa2b.defs.h: fa2b.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
fa2b.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
fa2b.gui.cpp: fa2b.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

pad2bit.analogfunction.cpp: analogfunction.xml
pad2bit.analogfunction.cpp: pad2bit.va
	$(ADMSXML) $< -e analogfunction.xml
pad2bit.core.cpp: pad2bit.defs.h qucsVersion.xml qucsMODULEcore.xml
pad2bit.core.cpp: pad2bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
pad2bit.defs.h: qucsVersion.xml qucsMODULEdefs.xml
pad2bit.defs.h: pad2bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
pad2bit.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
pad2bit.gui.cpp: pad2bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

pad3bit.analogfunction.cpp: analogfunction.xml
pad3bit.analogfunction.cpp: pad3bit.va
	$(ADMSXML) $< -e analogfunction.xml
pad3bit.core.cpp: pad3bit.defs.h qucsVersion.xml qucsMODULEcore.xml
pad3bit.core.cpp: pad3bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
pad3bit.defs.h: qucsVersion.xml qucsMODULEdefs.xml
pad3bit.defs.h: pad3bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
pad3bit.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
pad3bit.gui.cpp: pad3bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

pad4bit.analogfunction.cpp: analogfunction.xml
pad4bit.analogfunction.cpp: pad4bit.va
	$(ADMSXML) $< -e analogfunction.xml
pad4bit.core.cpp: pad4bit.defs.h qucsVersion.xml qucsMODULEcore.xml
pad4bit.core.cpp: pad4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
pad4bit.defs.h: qucsVersion.xml qucsMODULEdefs.xml
pad4bit.defs.h: pad4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
pad4bit.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
pad4bit.gui.cpp: pad4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

binarytogrey4bit.analogfunction.cpp: analogfunction.xml
binarytogrey4bit.analogfunction.cpp: binarytogrey4bit.va
	$(ADMSXML) $< -e analogfunction.xml
binarytogrey4bit.core.cpp: binarytogrey4bit.defs.h qucsVersion.xml qucsMODULEcore.xml
binarytogrey4bit.core.cpp: binarytogrey4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
binarytogrey4bit.defs.h: qucsVersion.xml qucsMODULEdefs.xml
binarytogrey4bit.defs.h: binarytogrey4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
binarytogrey4bit.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
binarytogrey4bit.gui.cpp: binarytogrey4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

greytobinary4bit.analogfunction.cpp: analogfunction.xml
greytobinary4bit.analogfunction.cpp: greytobinary4bit.va
	$(ADMSXML) $< -e analogfunction.xml
greytobinary4bit.core.cpp: greytobinary4bit.defs.h qucsVersion.xml qucsMODULEcore.xml
greytobinary4bit.core.cpp: greytobinary4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
greytobinary4bit.defs.h: qucsVersion.xml qucsMODULEdefs.xml
greytobinary4bit.defs.h: greytobinary4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
greytobinary4bit.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
greytobinary4bit.gui.cpp: greytobinary4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

comp_1bit.analogfunction.cpp: analogfunction.xml
comp_1bit.analogfunction.cpp: comp_1bit.va
	$(ADMSXML) $< -e analogfunction.xml
comp_1bit.core.cpp: comp_1bit.defs.h qucsVersion.xml qucsMODULEcore.xml
comp_1bit.core.cpp: comp_1bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
comp_1bit.defs.h: qucsVersion.xml qucsMODULEdefs.xml
comp_1bit.defs.h: comp_1bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
comp_1bit.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
comp_1bit.gui.cpp: comp_1bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

comp_2bit.analogfunction.cpp: analogfunction.xml
comp_2bit.analogfunction.cpp: comp_2bit.va
	$(ADMSXML) $< -e analogfunction.xml
comp_2bit.core.cpp: comp_2bit.defs.h qucsVersion.xml qucsMODULEcore.xml
comp_2bit.core.cpp: comp_2bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
comp_2bit.defs.h: qucsVersion.xml qucsMODULEdefs.xml
comp_2bit.defs.h: comp_2bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
comp_2bit.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
comp_2bit.gui.cpp: comp_2bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

comp_4bit.analogfunction.cpp: analogfunction.xml
comp_4bit.analogfunction.cpp: comp_4bit.va
	$(ADMSXML) $< -e analogfunction.xml
comp_4bit.core.cpp: comp_4bit.defs.h qucsVersion.xml qucsMODULEcore.xml
comp_4bit.core.cpp: comp_4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
comp_4bit.defs.h: qucsVersion.xml qucsMODULEdefs.xml
comp_4bit.defs.h: comp_4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
comp_4bit.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
comp_4bit.gui.cpp: comp_4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

hpribin4bit.analogfunction.cpp: analogfunction.xml
hpribin4bit.analogfunction.cpp: hpribin4bit.va
	$(ADMSXML) $< -e analogfunction.xml
hpribin4bit.core.cpp: hpribin4bit.defs.h qucsVersion.xml qucsMODULEcore.xml
hpribin4bit.core.cpp: hpribin4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
hpribin4bit.defs.h: qucsVersion.xml qucsMODULEdefs.xml
hpribin4bit.defs.h: hpribin4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
hpribin4bit.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
hpribin4bit.gui.cpp: hpribin4bit.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

GENERATED_FILES = *.core.* *.analogfunction.* *.gui.* *.defs.* .*.adms .*.xml
else
GENERATED_FILES =
endif

CLEANFILES = *~ *.orig *.rej $(GENERATED_FILES)

MAINTAINERCLEANFILES = Makefile.in
