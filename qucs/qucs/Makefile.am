## Process this file with automake to produce Makefile.in
#
# qucs/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
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
	components diagrams \
	paintings \
	dialogs \
	octave \
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

MOCHEADERS = \
  messagedock.h \
  octave_window.h \
  projectView.h \
  schematic.h \
  textdoc.h \
  qucs.h

MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

pqucsdir=${pkglibdir}
pqucs_LTLIBRARIES = \
	libqucs.la \
	libqucsschematic.la

libqucs_la_SOURCES = \
	attach.cpp \
	element.cpp \
	globals.cpp \
	imagewriter.cpp \
	io_error.cpp \
	messagedock.cpp \
	misc.cpp \
	mnemo.cpp \
	module.cpp \
	mouseactions.cpp \
	node.cpp \
	octave_window.cpp \
	printerwriter.cpp \
	projectView.cpp
	qucs_actions.cpp \
	qucs.cpp \
	qucsdoc.cpp \
	qucs_init.cpp \
	schematic.cpp \
	schematic_element.cpp \
	schematic_file.cpp \
	syntax.cpp \
	textdoc.cpp \
	viewpainter.cpp \
	wire.cpp \
	wirelabel.cpp

libqucs_la_SOURCES += qucs_.qrc

libqucsschematic_la_SOURCES += $(MOCFILES)

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS) -I$(top_srcdir)/qucs-lib
libqucsschematic_la_LDFLAGS = $(X11_LDFLAGS) $(QT_LIBS)

# these are not plugins yet. but there's hope.
# TODO:
# - cleanup
# - use attach, do not mention here.
PLUGINS = \
	components/libcomponents.la diagrams/libdiagrams.la \
	paintings/libpaintings.la dialogs/libdialogs.la

if COND_WIN32
# non-posix woes.
libqucs_la_LIBADD = \
	libqucsschematic.la
libqucsschematic_la_LIBADD = \
	libqucs.la \
	$(X11_LIBS) \
	$(QT_LIBS)
else
# link order hack?
libqucs_la_LIBADD =
libqucsschematic_la_LIBADD = # libqucsgui.la
endif

noinst_HEADERS = $(MOCHEADERS) wire.h qucsdoc.h element.h node.h \
  wirelabel.h viewpainter.h mnemo.h mouseactions.h syntax.h module.h misc.h \
  projectView.h printerwriter.h imagewriter.h command.h

# must be installed. but later
noinst_HEADERS += \
	globals.h \
	io_trace.h \
	l_dispatcher.h \
	object.h \
	platform.h

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


CLEANFILES = *~ *.rej *.orig qucs_.cpp *.moc.cpp qucs
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp *.qm
DISTCLEANFILES = *.qm
