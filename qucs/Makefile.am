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
  qucs.cpp main.cpp wirelabel.cpp qucsfile.cpp qucsinit.cpp qucsactions.cpp \
  viewpainter.cpp

nodist_qucs_SOURCES = $(MOCFILES)

INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES)
qucs_LDFLAGS = $(X11_LDFLAGS) $(QT_LDFLAGS)
qucs_LDADD = components/libcomponents.a diagrams/libdiagrams.a \
  paintings/libpaintings.a dialogs/libdialogs.a

noinst_HEADERS = $(MOCHEADERS) main.h wire.h qucsdoc.h element.h \
  node.h wirelabel.h qucsfile.h viewpainter.h

# rules for translations
TRANSLATIONS = qucs_de.ts qucs_pl.ts qucs_fr.ts qucs_it.ts qucs_ro.ts \
  qucs_pt.ts qucs_es.ts qucs_jp.ts qucs_hu.ts
GENTRANSLATIONS = qtgeneric_untranslated.ts qtgeneric_de.ts qtgeneric_fr.ts \
  qtgeneric_es.ts qtgeneric_ro.ts qtgeneric_jp.ts qtgeneric_hu.ts
TRANSFILES = $(TRANSLATIONS:.ts=.qm)

qucs_%.ts: $(qucs_SOURCES)
	@echo "Updating translations...";
	@list='$(TRANSLATIONS)'; for file in $$list; do \
	  $(LUPDATE) `$(PATHXFORM) $(srcdir)/*.cpp $(srcdir)/paintings/*.cpp \
	  $(srcdir)/diagrams/*.cpp $(srcdir)/components/*.cpp \
	  $(srcdir)/dialogs/*.cpp  $(top_srcdir)/qucs-help/*.cpp \
	  $(top_srcdir)/qucs-edit/*.cpp` -ts $(srcdir)/$$file; \
	done

%.qm: %.ts
	@qucsfile=$<; tmpfile="tmp.ts"; \
	genfile=`echo $< | sed -e 's/qucs_\(..\)\.ts/qtgeneric_\1\.ts/'`; \
	if test -f $$genfile; then \
	  echo "Incorporating '$$genfile'..."; \
	  grep -v "</TS>" $$qucsfile > $$tmpfile; \
	  grep -v "<\!DOCTYPE TS><TS>" $$genfile >> $$tmpfile; \
	else \
	  cp -f $$qucsfile $$tmpfile; \
	fi; \
	echo "$(LRELEASE) $$qucsfile"; \
	$(LRELEASE) $$tmpfile; mv -f tmp.qm $@; rm -f $$tmpfile

# installation of translations
langdatadir = ${prefix}/share/qucs/lang
langdata_DATA = $(TRANSFILES)

EXTRA_DIST = $(TRANSLATIONS) $(GENTRANSLATIONS) README.translations

all-local: $(TRANSLATIONS) $(TRANSFILES)

%.moc.cpp: %.h
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp *.qm
DISTCLEANFILES = *.moc.cpp *.qm
