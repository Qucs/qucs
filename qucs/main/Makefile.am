
qucs_appledir = $(bindir)/qucs.app/Contents/MacOS/

include ../common.mk

# why this? qucs_real_LDADD does not seem to work on windows (yet?)

qucs_real_LDFLAGS = $(QT_LDFLAGS)
qucs_real_SOURCES = main.cpp # qucs_.qrc
qucs_real_CPPFLAGS = ${AM_CPPFLAGS} $(QT_CFLAGS) -I${top_srcdir}/qucs

# these are not plugins yet. but there's hope.
# TODO:
# - cleanup
# - use attach, do not mention here.
PLUGINS = \
	../qucs/components/libcomponents.la \
	../qucs/diagrams/libdiagrams.la \
	../qucs/paintings/libpaintings.la \
	../qucs/dialogs/libdialogs.la


if COND_WIN32
qucs_real_LDADD = \
../qucs/libqucsgui.la \
	../qucs/libqucsschematic.la ${PLUGINS}
else
qucs_real_LDADD = \
../qucs/libqucsgui.la \
	../qucs/libqucsschematic.la ${PLUGINS} \
	$(X11_LIBS) $(QT_LIBS)
endif

if COND_MACOSX
qucs_apple_PROGRAMS = qucs.real
else
endif

bin_PROGRAMS = qucs.real
transform=s&\.real$$&&;$(program_transform_name)

CLEANFILES = qucs_.cpp
