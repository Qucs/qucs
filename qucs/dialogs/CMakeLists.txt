# qucs/dialogs library

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}) # ui_*.h, out of tree

# INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

set(DIALOGS_HDRS
    aboutdialog.h
    changedialog.h
    digisettingsdialog.h
    exportdialog.h
    importdialog.h
    labeldialog.h
    librarydialog.h
    loaddialog.h
    matchdialog.h
    newprojdialog.h
    packagedialog.h
    qucssettingsdialog.h
    savedialog.h
    searchdialog.h
    settingsdialog.h
    simmessage.h
    sweepdialog.h
    vasettingsdialog.h)

set(DIALOGS_SRCS
    changedialog.cpp
    packagedialog.cpp
    vasettingsdialog.cpp
    digisettingsdialog.cpp
    qucssettingsdialog.cpp
    importdialog.cpp
    savedialog.cpp
    labeldialog.cpp
    searchdialog.cpp
    librarydialog.cpp
    settingsdialog.cpp
    matchdialog.cpp
    simmessage.cpp
    newprojdialog.cpp
    sweepdialog.cpp
    exportdialog.cpp
    loaddialog.cpp
    aboutdialog.cpp)

set(DIALOGS_MOC_HDRS
    aboutdialog.h
    changedialog.h
    digisettingsdialog.h
    exportdialog.h
    importdialog.h
    labeldialog.h
    librarydialog.h
    loaddialog.h
    matchdialog.h
    newprojdialog.h
    packagedialog.h
    qucssettingsdialog.h
    savedialog.h
    searchdialog.h
    settingsdialog.h
    simmessage.h
    sweepdialog.h
    sweepdialog.h
    vasettingsdialog.h)

set(DIALOGS_UIC_HDRS searchdialog.ui)

qt4_wrap_cpp(DIALOGS_MOC_SRCS ${DIALOGS_MOC_HDRS})

qt4_wrap_ui(DIALOGS_UIC_SRCS ${DIALOGS_UIC_HDRS})

add_library(dialogs OBJECT ${DIALOGS_HDRS} ${DIALOGS_SRCS} ${DIALOGS_MOC_SRCS}
                           ${DIALOGS_UIC_SRCS})
