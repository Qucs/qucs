# qucs/diagrams library

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(DIAGRAMS_HDRS
curvediagram.h
diagram.h
diagramdialog.h
diagrams.h
graph.h
marker.h
markerdialog.h
#phasordiagram.h
polardiagram.h
psdiagram.h
rect3ddiagram.h
rectdiagram.h
smithdiagram.h
tabdiagram.h
timingdiagram.h
truthdiagram.h
#waveac.h
)

SET(DIAGRAMS_SRCS
curvediagram.cpp	graph.cpp		polardiagram.cpp	smithdiagram.cpp
diagram.cpp		marker.cpp		psdiagram.cpp		tabdiagram.cpp
diagramdialog.cpp	markerdialog.cpp	rect3ddiagram.cpp	timingdiagram.cpp
rectdiagram.cpp
#phasordiagram.cpp
truthdiagram.cpp
#waveac.cpp
)

SET(DIAGRAMS_MOC_HDRS
diagramdialog.h
markerdialog.h
)

IF(${USE_QT_VERSION} MATCHES "4")
    QT4_WRAP_CPP( DIAGRAMS_MOC_SRCS ${DIAGRAMS_MOC_HDRS} )
ELSE()
    QT5_WRAP_CPP( DIAGRAMS_MOC_SRCS ${DIAGRAMS_MOC_HDRS} )
ENDIF()

ADD_LIBRARY(diagrams OBJECT ${DIAGRAMS_HDRS} ${DIAGRAMS_SRCS} ${DIAGRAMS_MOC_SRCS})
