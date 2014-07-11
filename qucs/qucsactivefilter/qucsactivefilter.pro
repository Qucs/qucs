#-------------------------------------------------
#
# Project created by QtCreator 2014-04-10T18:36:21
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qucsactivefilter
TEMPLATE = app


SOURCES += main.cpp\
    filter.cpp \
    sallenkey.cpp \
    mfbfilter.cpp \
    qf_poly.cpp \
    schcauer.cpp \
    transferfuncdialog.cpp \
    qucsactivefilter.cpp

HEADERS  += \
    filter.h \
    sallenkey.h \
    mfbfilter.h \
    qf_poly.h \
    qf_matrix.h \
    schcauer.h \
    transferfuncdialog.h \
    bessel.h \
    qucsactivefilter.h

RESOURCES += \
    qucsactivefilter.qrc
