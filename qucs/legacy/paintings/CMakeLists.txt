# qucs/paintings library

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

find_package(
  Qt5
  REQUIRED
  Widgets
  )

set(PAINTINGS_HDRS
    arrowdialog.h
    filldialog.h
    graphictext.h
    graphictextdialog.h
    )

set(PAINTINGS_SRCS
    arrow.cpp
    arrowdialog.cpp
    graphicline.cpp
    graphictext.cpp
    ellipse.cpp
    graphictextdialog.cpp
    portsymbol.cpp
    ellipsearc.cpp
    rectangle.cpp
    filldialog.cpp)

add_library(paintings SHARED ${PAINTINGS_HDRS} ${PAINTINGS_SRCS}
                             ${PAINTINGS_MOC_SRCS})
target_include_directories(paintings
	PRIVATE
	${qucs_app_SOURCE_DIR}/../../src
	${qucs_app_SOURCE_DIR}/../../include)
target_link_libraries(paintings Qt5::Widgets)
