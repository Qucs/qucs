## Process this file with automake to produce Makefile.in
# 
# src/components/devices/Makefile.am
#
# Automake input file for the component library.
#

  eqndefined.cpp diac.cpp thyristor.cpp triac.cpp tunneldiode.cpp

noinst_HEADERS = device.h diode.h jfet.h bjt.h mosfet.h eqndefined.h diac.h \
  thyristor.h triac.h tunneldiode.h


INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/math \
  -I$(top_srcdir)/src/components

CLEANFILES = *~ *.orig *.rej

MAINTAINERCLEANFILES = Makefile.in
