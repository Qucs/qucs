####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = qucs

## INCLUDES were found outside kdevelop specific part

qucs_SOURCES = fileshowdialog.cpp messagebox.cpp settingsdialog.cpp node.cpp element.cpp simmessage.cpp qucsdoc.cpp wire.cpp newprojdialog.cpp qucsview.cpp qucs.cpp main.cpp 
SOURCES = paintings/rectangle.cpp paintings/painting.cpp paintings/linedialog.cpp paintings/graphictextdialog.cpp paintings/graphictext.cpp paintings/graphicline.cpp paintings/ellipse.cpp paintings/arrowdialog.cpp paintings/arrow.cpp diagrams/tabdiagram.cpp diagrams/smithdiagram.cpp diagrams/rectdiagram.cpp diagrams/polardiagram.cpp diagrams/graph.cpp diagrams/diagramdialog.cpp diagrams/diagram.cpp components/phaseshifter.cpp components/gyrator.cpp components/componentdialog.cpp components/vcvs.cpp components/vccs.cpp components/tr_sim.cpp components/tline.cpp components/substrate.cpp components/sp_sim.cpp components/param_sweep.cpp components/mstee.cpp components/msstep.cpp components/msline.cpp components/mscross.cpp components/mscoupled.cpp components/mscorner.cpp components/hb_sim.cpp components/diode.cpp components/dc_sim.cpp components/coplanar.cpp components/ccvs.cpp components/cccs.cpp components/ac_sim.cpp components/volt_dc.cpp components/volt_ac.cpp components/subcircuit.cpp components/sparamfile.cpp components/source_ac.cpp components/isolator.cpp components/equation.cpp components/circulator.cpp components/attenuator.cpp components/ampere_dc.cpp components/transformer.cpp components/symtrafo.cpp components/subcirport.cpp components/ground.cpp components/dcfeed.cpp components/dcblock.cpp components/biast.cpp components/inductor.cpp components/capacitor.cpp components/resistorus.cpp components/component.cpp components/resistor.cpp sim/complex.cpp fileshowdialog.cpp messagebox.cpp settingsdialog.cpp node.cpp element.cpp simmessage.cpp qucsdoc.cpp wire.cpp newprojdialog.cpp qucsview.cpp qucs.cpp main.cpp 
INTERFACES = 
TRANSLATIONS = 
KDE_OPTIONS = qtonly
qucs_LDADD   = ./sim/libsim.a ./components/libcomponents.a ./diagrams/libdiagrams.a ./paintings/libpaintings.a $(LIB_QPE) $(LIB_QT) $(LIBSOCKET)

SUBDIRS = bitmaps sim components diagrams paintings 

EXTRA_DIST = main.cpp qucs.cpp qucs.h qucsview.cpp qucsview.h lo32-app-qucs.png lo16-app-qucs.png newprojdialog.cpp newprojdialog.h wire.cpp wire.h qucsdoc.cpp qucsdoc.h simmessage.cpp simmessage.h element.cpp element.h node.cpp node.h settingsdialog.cpp settingsdialog.h messagebox.cpp messagebox.h fileshowdialog.cpp fileshowdialog.h 

####### kdevelop will overwrite this part!!! (end)############


# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

qucs_METASOURCES = AUTO

# the library search path. 
qucs_LDFLAGS = $(all_libraries) 

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(qucs_METASOURCES)

# installation of translations
langdatadir = ${prefix}/share/qucs/lang
langdata_DATA = 

dist-hook:
	rm -f $(distdir)/$(bin_PROGRAMS)
