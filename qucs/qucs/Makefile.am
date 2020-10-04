## Process this file with automake to produce Makefile.in
#
# qucs/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004-2011  Stefan Jahn <stefan@lkcc.org>
#               2018, 2020 Felix Salfelder
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

include ../common.mk

AM_DEFAULT_SOURCE_EXT = .cpp

SUBDIRS = \
	. \
	bitmaps \
	components \
	diagrams \
	paintings \
	octave \
	legacy \
	sim \
	tests

all-local: qucs

qucs: Makefile
	echo '#!/bin/sh' > $@
	echo 'exec ${abs_top_builddir}/main/qucs "$$@"' >> $@
	chmod +x $@

// todo: move to configure
if COND_WIN32
BAT_EXT = .bat
else
BAT_EXT =
endif

bin_SCRIPTS = qucsdigi${BAT_EXT} qucsveri${BAT_EXT} qucsdigilib${BAT_EXT}

DIALG_MOCHEADERS = \
	aboutdialog.h \
	changedialog.h \
	diagramdialog.h \
	digisettingsdialog.h \
	exportdialog.h \
	importdialog.h \
	labeldialog.h \
	librarydialog.h \
	loaddialog.h \
	matchdialog.h \
	newprojdialog.h \
	packagedialog.h \
	qucssettingsdialog.h \
	savedialog.h \
	searchdialog.h \
	settingsdialog.h \
	simmessage.h \
	sweepdialog.h \
	vasettingsdialog.h

UICFILES = ui_searchdialog.h
BUILT_SOURCES = $(UICFILES)

# common?!
ui_searchdialog.h: searchdialog.ui
	$(UIC) -o $@ $<
ui_searchdialog.h: $(top_builddir)/qt_version

MOCHEADERS = \
	${DIALG_MOCHEADERS} \
	${QMOCHEADERS}

MOCCPP = $(MOCHEADERS:.h=.moc.cpp)

pqucsdir=${pkglibdir}
pqucs_LTLIBRARIES = \
	libqucs.la

# dialogs related to components
# sort out later.
DIALOG_SRC = \
	componentdialog.cpp \
	diagramdialog.cpp \
	optimizedialog.cpp

# plugins? lets see
DIALOG_SRC+= \
								settingsdialog.cpp newprojdialog.cpp \
     simmessage.cpp qucssettingsdialog.cpp labeldialog.cpp changedialog.cpp \
     matchdialog.cpp sweepdialog.cpp digisettingsdialog.cpp searchdialog.cpp \
     librarydialog.cpp importdialog.cpp packagedialog.cpp \
     savedialog.cpp vasettingsdialog.cpp exportdialog.cpp loaddialog.cpp \
     aboutdialog.cpp


include MakeList
libqucs_la_SOURCES = \
	${CPP_SRC} \
	$(MOCCPP) \
	${DIALOG_SRC}

libqucs_la_SOURCES += qucs_.qrc


AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS) -I$(top_srcdir)/qucs-lib

noinst_HEADERS = \
	actions.h \
	command.h \
	element.h \
	exception.h \
	imagewriter.h \
  	imagewriter.h \
	language.h \
	misc.h \
	mnemo.h \
	module.h \
	port.h \
	node.h \
	nodemap.h \
	printerwriter.h \
	projectView.h \
	qt_compat.h \
	qucsdoc.h \
	symbol.h \
	syntax.h \
	schematic_lang.h \
	schematic_symbol.h \
	some_font_stuff.h \
	trace.h \
	viewpainter.h \
	wire.h \
	wirelabel.h \
	$(MOCHEADERS)

# must be installed. but later
noinst_HEADERS += \
	actions.h \
	command.h \
	diagram.h \
	dispatcher.h \
	globals.h \
	imagewriter.h \
	io.h \
	io_trace.h \
	l_dispatcher.h \
	misc.h \
	mnemo.h \
	module.h \
	mouseactions.h \
	object.h \
	platform.h \
	printerwriter.h \
	projectView.h \
	qt_compat.h \
	schematic_lang.h \
	schematic_model.h \
	schematic_symbol.h \
	syntax.h \
	trace.h \
	viewpainter.h \
	wirelabel.h \
	wirelabel.h

hicolor16dir = ${prefix}/share/icons/hicolor/16x16/apps
hicolor16_DATA =  bitmaps/hicolor/16x16/apps/qucs.png
hicolor22dir = ${prefix}/share/icons/hicolor/22x22/apps
hicolor22_DATA =bitmaps/hicolor/22x22/apps/qucs.png
hicolor32dir = ${prefix}/share/icons/hicolor/32x32/apps
hicolor32_DATA =bitmaps/hicolor/32x32/apps/qucs.png
hicolor48dir = ${prefix}/share/icons/hicolor/48x48/apps
hicolor48_DATA =bitmaps/hicolor/48x48/apps/qucs.png
hicolor64dir = ${prefix}/share/icons/hicolor/64x64/apps
hicolor64_DATA =bitmaps/hicolor/64x64/apps/qucs.png
hicolor128dir = ${prefix}/share/icons/hicolor/128x128/apps
hicolor128_DATA =bitmaps/hicolor/128x128/apps/qucs.png
hicolor256dir = ${prefix}/share/icons/hicolor/256x256/apps
hicolor256_DATA =bitmaps/hicolor/256x256/apps/qucs.png
hicolor512dir = ${prefix}/share/icons/hicolor/512x512/apps
hicolor512_DATA =bitmaps/hicolor/512x512/apps/qucs.png
hicolorscdir = ${prefix}/share/icons/hicolor/scalable/apps
hicolorsc_DATA =bitmaps/hicolor/scalable/apps/qucs.svg


desktopdir = ${prefix}/share/applications
desktop_DATA = qucs.desktop


EXTRA_DIST = \
	searchdialog.ui \
	qucsdigi qucsdigilib qucsdigi.bat qucsdigilib.bat qucsveri qucsveri.bat \
	$(hicolor16_DATA) $(hicolor22_DATA) $(hicolor32_DATA) $(hicolor48_DATA) \
	$(hicolor64_DATA) $(hicolor128_DATA) $(hicolor256_DATA) $(hicolor512_DATA) \
	$(hicolorsc_DATA) $(desktop_DATA) \
	bitmaps/qucs.icns \
	bitmaps/qucsattenuator.icns \
	bitmaps/qucsfilter.icns \
	bitmaps/qucsactivefilter.icns \
	bitmaps/qucshelp.icns \
	bitmaps/qucslib.icns \
	bitmaps/qucsedit.icns \
	bitmaps/qucsrescodes.icns \
	bitmaps/qucstrans.icns


CLEANFILES = *~ *.rej *.orig qucs_.cpp *.moc.cpp qucs \
	${UICFILES}
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp *.qm
DISTCLEANFILES = *.qm
