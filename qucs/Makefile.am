####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = qucs

## INCLUDES were found outside kdevelop specific part

qucs_SOURCES = thing.cpp node.cpp element.cpp componentdialog.cpp simmessage.cpp qucsdoc.cpp wire.cpp component.cpp newprojdialog.cpp qucsview.cpp qucs.cpp main.cpp 
SOURCES = thing.cpp node.cpp element.cpp componentdialog.cpp simmessage.cpp qucsdoc.cpp wire.cpp component.cpp newprojdialog.cpp qucsview.cpp qucs.cpp main.cpp 
INTERFACES = 
TRANSLATIONS = 
KDE_OPTIONS = qtonly
qucs_LDADD   =  $(LIB_QPE) $(LIB_QT) $(LIBSOCKET)

SUBDIRS = bitmaps 

EXTRA_DIST = main.cpp qucs.cpp qucs.h qucsview.cpp qucsview.h lo32-app-qucs.png lo16-app-qucs.png newprojdialog.cpp newprojdialog.h component.cpp component.h wire.cpp wire.h qucsdoc.cpp qucsdoc.h simmessage.cpp simmessage.h componentdialog.cpp componentdialog.h element.cpp element.h node.cpp node.h thing.cpp thing.h 

####### kdevelop will overwrite this part!!! (end)############


# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

qucs_METASOURCES = AUTO

# the library search path. 
qucs_LDFLAGS = $(all_libraries) 

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(qucs_METASOURCES)

