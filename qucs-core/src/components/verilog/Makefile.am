## Process this file with automake to produce Makefile.in
# 
# src/components/verilog/Makefile.am
#
# Automake input file for the verilog device library.
#
# Copyright (C) 2006, 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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
	hicumL2p1.analogfunction.cpp hicumL2p1.core.cpp \
	mod_amp.analogfunction.cpp mod_amp.core.cpp \
	hic2_full.analogfunction.cpp hic2_full.core.cpp \
	log_amp.analogfunction.cpp log_amp.core.cpp \
	hic0_full.analogfunction.cpp hic0_full.core.cpp \
	potentiometer.analogfunction.cpp potentiometer.core.cpp \
	MESFET.analogfunction.cpp MESFET.core.cpp \
	EKV26nMOS.analogfunction.cpp EKV26nMOS.core.cpp \
	EKV26pMOS.analogfunction.cpp EKV26pMOS.core.cpp

noinst_HEADERS = HBT_X.analogfunction.h HBT_X.core.h \
	hicumL2p1.analogfunction.h hicumL2p1.core.h \
	mod_amp.analogfunction.h mod_amp.core.h \
	hic2_full.analogfunction.h hic2_full.core.h \
	log_amp.analogfunction.h log_amp.core.h \
	hic0_full.analogfunction.h hic0_full.core.h \
	potentiometer.analogfunction.h potentiometer.core.h \
	MESFET.analogfunction.h MESFET.core.h \
	EKV26nMOS.analogfunction.h EKV26nMOS.core.h \
	EKV26pMOS.analogfunction.h EKV26pMOS.core.h

VERILOG_FILES = constants.vams disciplines.vams \
  fbh_hbt-2_2a.va hicumL2V2p11.va mod_amp.va hicumL2V2p22.va log_amp.va \
  hicumL0V1p12.va potentiometer.va MESFET.va EKV26nMOS.va EKV26pMOS.va

XML_FILES = analogfunction.xml qucsVersion.xml qucsMODULEcore.xml \
  qucsMODULEdefs.xml qucsMODULEgui.xml

EXTRA_DIST = $(VERILOG_FILES) $(XML_FILES)

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/math \
  -I$(top_srcdir)/src/components -I$(top_srcdir)/src/components/devices

if MAINTAINER_MODE

HBT_X.analogfunction.cpp: analogfunction.xml
HBT_X.analogfunction.cpp: fbh_hbt-2_2a.va
	$(ADMSXML) $< -e analogfunction.xml
HBT_X.core.cpp: qucsVersion.xml qucsMODULEcore.xml
HBT_X.core.cpp: fbh_hbt-2_2a.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
HBT_X.defs.h: qucsVersion.xml qucsMODULEdefs.xml
HBT_X.defs.h: fbh_hbt-2_2a.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
HBT_X.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
HBT_X.gui.cpp: fbh_hbt-2_2a.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

hicumL2p1.analogfunction.cpp: analogfunction.xml
hicumL2p1.analogfunction.cpp: hicumL2V2p11.va
	$(ADMSXML) $< -e analogfunction.xml
hicumL2p1.core.cpp: qucsVersion.xml qucsMODULEcore.xml
hicumL2p1.core.cpp: hicumL2V2p11.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
hicumL2p1.defs.h: qucsVersion.xml qucsMODULEdefs.xml
hicumL2p1.defs.h: hicumL2V2p11.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
hicumL2p1.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
hicumL2p1.gui.cpp: hicumL2V2p11.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

mod_amp.analogfunction.cpp: analogfunction.xml
mod_amp.analogfunction.cpp: mod_amp.va
	$(ADMSXML) $< -e analogfunction.xml
mod_amp.core.cpp: qucsVersion.xml qucsMODULEcore.xml
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
hic2_full.core.cpp: qucsVersion.xml qucsMODULEcore.xml
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
log_amp.core.cpp: qucsVersion.xml qucsMODULEcore.xml
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
hic0_full.core.cpp: qucsVersion.xml qucsMODULEcore.xml
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
potentiometer.core.cpp: qucsVersion.xml qucsMODULEcore.xml
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
MESFET.core.cpp: qucsVersion.xml qucsMODULEcore.xml
MESFET.core.cpp: MESFET.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
MESFET.defs.h: qucsVersion.xml qucsMODULEdefs.xml
MESFET.defs.h: MESFET.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
MESFET.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
MESFET.gui.cpp: MESFET.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

EKV26nMOS.analogfunction.cpp: analogfunction.xml
EKV26nMOS.analogfunction.cpp: EKV26nMOS.va
	$(ADMSXML) $< -e analogfunction.xml
EKV26nMOS.core.cpp: qucsVersion.xml qucsMODULEcore.xml
EKV26nMOS.core.cpp: EKV26nMOS.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
EKV26nMOS.defs.h: qucsVersion.xml qucsMODULEdefs.xml
EKV26nMOS.defs.h: EKV26nMOS.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
EKV26nMOS.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
EKV26nMOS.gui.cpp: EKV26nMOS.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

EKV26pMOS.analogfunction.cpp: analogfunction.xml
EKV26pMOS.analogfunction.cpp: EKV26pMOS.va
	$(ADMSXML) $< -e analogfunction.xml
EKV26pMOS.core.cpp: qucsVersion.xml qucsMODULEcore.xml
EKV26pMOS.core.cpp: EKV26pMOS.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
EKV26pMOS.defs.h: qucsVersion.xml qucsMODULEdefs.xml
EKV26pMOS.defs.h: EKV26pMOS.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
EKV26pMOS.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
EKV26pMOS.gui.cpp: EKV26pMOS.va
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

GENERATED_FILES = *.core.* *.analogfunction.* *.gui.* *.defs.* .*.adms .*.xml
else
GENERATED_FILES =
endif

CLEANFILES = *~ *.orig *.rej $(GENERATED_FILES)

MAINTAINERCLEANFILES = Makefile.in
