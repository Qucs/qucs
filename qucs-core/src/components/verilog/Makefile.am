## Process this file with automake to produce Makefile.in
# 
# src/components/verilog/Makefile.am
#
# Automake input file for the verilog device library.
#
# Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
	hicumL2p1.analogfunction.cpp hicumL2p1.core.cpp

noinst_HEADERS = HBT_X.core.h HBT_X.analogfunction.h \
	hicumL2p1.core.h hicumL2p1.analogfunction.h

VERILOG_FILES = constants.vams disciplines.vams \
  fbh_hbt-2_2a.va hicumL2V2p11.va

XML_FILES = analogfunction.xml qucsVersion.xml qucsMODULEcore.xml \
  qucsMODULEdefs.xml qucsMODULEgui.xml

EXTRA_DIST = $(VERILOG_FILES) $(XML_FILES)

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/components -I$(top_srcdir)/src/components/devices

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

GENERATED_FILES = *.core.* *.analogfunction.* *.gui.* *.defs.* .*.adms .*.xml
else
GENERATED_FILES =
endif

CLEANFILES = *~ *.orig *.rej $(GENERATED_FILES)

MAINTAINERCLEANFILES = Makefile.in
