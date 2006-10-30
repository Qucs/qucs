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

libverilog_a_SOURCES = HBT_X.cpp HBT_X_analogfunction.cpp \
	hicumL2p1.cpp hicumL2p1_analogfunction.cpp

noinst_HEADERS = HBT_X.h HBT_X_analogfunction.h \
	hicumL2p1.h hicumL2p1_analogfunction.h

VERILOG_FILES = constants.vams disciplines.vams \
  fbh_hbt-2_2a.va hicumL2V2p11.va

XML_FILES = analogfunction.xml qucsVersion.xml qucsMODULEcore.xml \
  qucsMODULEdefs.xml qucsMODULEgui.xml

EXTRA_DIST = $(VERILOG_FILES) $(XML_FILES)

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/components -I$(top_srcdir)/src/components/devices

if MAINTAINER_MODE
HBT_X.cpp: fbh_hbt-2_2a.va
	$(ADMSXML) $< -e analogfunction.xml
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml
hicumL2p1.cpp: hicumL2V2p11.va
	$(ADMSXML) $< -e analogfunction.xml
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEcore.xml
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEdefs.xml
	$(ADMSXML) $< -e qucsVersion.xml -e qucsMODULEgui.xml

GENERATED_FILES = HBT_X* hicumL2p1* .*.adms .*.xml
else
GENERATED_FILES =
endif

CLEANFILES = *~ *.orig *.rej $(GENERATED_FILES)

MAINTAINERCLEANFILES = Makefile.in
