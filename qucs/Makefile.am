####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = qucs

## INCLUDES were found outside kdevelop specific part

qucs_SOURCES = linedialog.cpp graphictextdialog.cpp graphictext.cpp graphicline.cpp arrow.cpp ellipse.cpp rectangle.cpp painting.cpp fileshowdialog.cpp messagebox.cpp tabdiagram.cpp settingsdialog.cpp smithdiagram.cpp polardiagram.cpp diagramdialog.cpp rectdiagram.cpp diagram.cpp graph.cpp node.cpp element.cpp componentdialog.cpp simmessage.cpp qucsdoc.cpp wire.cpp component.cpp newprojdialog.cpp qucsview.cpp qucs.cpp main.cpp 
SOURCES = sim/complex.cpp linedialog.cpp graphictextdialog.cpp graphictext.cpp graphicline.cpp arrow.cpp ellipse.cpp rectangle.cpp painting.cpp fileshowdialog.cpp messagebox.cpp tabdiagram.cpp settingsdialog.cpp smithdiagram.cpp polardiagram.cpp diagramdialog.cpp rectdiagram.cpp diagram.cpp graph.cpp node.cpp element.cpp componentdialog.cpp simmessage.cpp qucsdoc.cpp wire.cpp component.cpp newprojdialog.cpp qucsview.cpp qucs.cpp main.cpp 
INTERFACES = 
TRANSLATIONS = 
KDE_OPTIONS = qtonly
qucs_LDADD   = ./sim/libsim.a $(LIB_QPE) $(LIB_QT) $(LIBSOCKET)

SUBDIRS = bitmaps sim 

EXTRA_DIST = main.cpp qucs.cpp qucs.h qucsview.cpp qucsview.h lo32-app-qucs.png lo16-app-qucs.png newprojdialog.cpp newprojdialog.h component.cpp component.h wire.cpp wire.h qucsdoc.cpp qucsdoc.h simmessage.cpp simmessage.h componentdialog.cpp componentdialog.h element.cpp element.h node.cpp node.h graph.cpp graph.h diagram.cpp diagram.h rectdiagram.cpp rectdiagram.h diagramdialog.cpp diagramdialog.h polardiagram.cpp polardiagram.h smithdiagram.cpp smithdiagram.h settingsdialog.cpp settingsdialog.h tabdiagram.cpp tabdiagram.h messagebox.cpp messagebox.h fileshowdialog.cpp fileshowdialog.h painting.cpp painting.h rectangle.cpp rectangle.h ellipse.cpp ellipse.h arrow.cpp arrow.h graphicline.cpp graphicline.h graphictext.cpp graphictext.h graphictextdialog.cpp graphictextdialog.h linedialog.cpp linedialog.h 

####### kdevelop will overwrite this part!!! (end)############


# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

qucs_METASOURCES = AUTO

# the library search path. 
qucs_LDFLAGS = $(all_libraries) 

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(qucs_METASOURCES)

dist-hook:
	rm -f $(distdir)/$(bin_PROGRAMS)
