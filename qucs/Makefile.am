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
	bitmaps \
	components diagrams \
	paintings \
	dialogs \
	octave \
	tests

all-local: qucs

if COND_WIN32
bin_SCRIPTS = qucsdigi.bat qucsveri.bat qucsdigilib.bat
else
bin_SCRIPTS = qucsdigi qucsveri qucsdigilib
endif

qucs_appledir = $(bindir)/qucs.app/Contents/MacOS/

if COND_MACOSX
qucs_apple_PROGRAMS = qucs.real
else
endif

bin_PROGRAMS = qucs.real
transform=s&\.real$$&&;$(program_transform_name)

MOCHEADERS = \
  messagedock.h \
  octave_window.h \
  projectView.h \
  schematic.h \
  textdoc.h \
  qucs.h

MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

noinst_LTLIBRARIES = libqucsschematic.la

libqucsschematic_la_SOURCES= \
  attach.cpp \
  node.cpp element.cpp qucsdoc.cpp wire.cpp mouseactions.cpp   \
  qucs.cpp wirelabel.cpp qucs_init.cpp qucs_actions.cpp            \
  viewpainter.cpp mnemo.cpp schematic.cpp schematic_element.cpp textdoc.cpp \
  schematic_file.cpp syntax.cpp module.cpp octave_window.cpp \
  messagedock.cpp misc.cpp imagewriter.cpp printerwriter.cpp \
  projectView.cpp

nodist_libqucsschematic_la_SOURCES = $(MOCFILES)

include ../Makefile.common
libqucsschematic_la_LDFLAGS = $(X11_LDFLAGS) $(QT_LIBS)

# TODO:
# - parts must go to libqucs
# - other parts need to be loaded at run time
libqucsschematic_la_LIBADD = \
 components/libcomponents.la diagrams/libdiagrams.la \
  paintings/libpaintings.la dialogs/libdialogs.la \
  ../qt3_compat/libqucsqt3compat.la

# why this? qucs_real_LDADD does not seem to work on windows (yet?)
libqucsschematic_la_LIBADD += \
  $(X11_LIBS) $(QT_LIBS)

qucs_real_LDFLAGS = $(QT_LIBS)
qucs_real_SOURCES = main.cpp qucs_.qrc
qucs_real_LDADD = \
	libqucsschematic.la

noinst_HEADERS = $(MOCHEADERS) wire.h qucsdoc.h element.h node.h \
  wirelabel.h viewpainter.h mnemo.h mouseactions.h syntax.h module.h misc.h \
  projectView.h printerwriter.h imagewriter.h

# must be installed. but later
noinst_HEADERS += platform.h

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


CLEANFILES = *~ *.rej *.orig qucs_.cpp *.moc.cpp
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp *.qm
DISTCLEANFILES = *.qm
