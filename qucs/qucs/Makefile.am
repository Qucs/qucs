## Process this file with automake to produce Makefile.in
# 
# qucs/Makefile.am
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

SUBDIRS = bitmaps components diagrams paintings dialogs docs
bin_PROGRAMS = qucs

MOCHEADERS = qucsview.h qucs.h qucsinit.h qucsactions.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

qucs_SOURCES = node.cpp element.cpp qucsdoc.cpp wire.cpp qucsview.cpp \
  qucs.cpp main.cpp wirelabel.cpp qucsfile.cpp qucsinit.cpp qucsactions.cpp

nodist_qucs_SOURCES = $(MOCFILES)

INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES)
qucs_LDFLAGS = $(X11_LDFLAGS) $(QT_LDFLAGS)
qucs_LDADD = components/libcomponents.a diagrams/libdiagrams.a \
  paintings/libpaintings.a dialogs/libdialogs.a

noinst_HEADERS = $(MOCHEADERS) main.h wire.h qucsdoc.h element.h \
  node.h wirelabel.h qucsfile.h

# rules for translations
TRANSLATIONS = $(srcdir)/qucs_de.ts $(srcdir)/qucs_pl.ts $(srcdir)/qucs_fr.ts \
  $(srcdir)/qucs_it.ts $(srcdir)/qucs_ro.ts
TRANSFILES = $(TRANSLATIONS:.ts=.qm)

%.ts: $(qucs_SOURCES)
	@echo "Updating translations...";
	@list='$(TRANSLATIONS)'; for file in $$list; do \
	  $(LUPDATE) `$(PATHXFORM) $(srcdir)/*.cpp $(srcdir)/paintings/*.cpp \
	  $(srcdir)/diagrams/*.cpp $(srcdir)/components/*.cpp \
	  $(srcdir)/dialogs/*.cpp` -ts $$file; \
	done

%.qm: %.ts
	$(LRELEASE) $<

# installation of translations
langdatadir = ${prefix}/share/qucs/lang
langdata_DATA = $(TRANSFILES)

EXTRA_DIST = $(TRANSLATIONS)

all-local: $(TRANSLATIONS) $(TRANSFILES)

%.moc.cpp: %.h
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp *.qm
DISTCLEANFILES = *.moc.cpp *.qm
