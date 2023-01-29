
noinst_LTLIBRARIES = libqucsqt3compat.la

libqucsqt3compat_la_SOURCES = \
	q3frame.cpp \
	q3gdict.cpp \
	q3gvector.cpp \
	q3glist.cpp \
	q3scrollview.cpp

nodist_libqucsqt3compat_la_SOURCES = $(MOCFILES)

MOCHEADERS = \
	q3scrollview.h q3frame.h

MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

include ../common.mk
include ../Makefile.common
