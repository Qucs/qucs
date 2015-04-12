# qucs/dialogs library

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR} ) # ui_*.h, out of tree

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(DIALOGS_HDRS
changedialog.h
digisettingsdialog.h
exportdialog.h
labeldialog.h
loaddialog.h
newprojdialog.h
savedialog.h
searchdialog.h
settingsdialog.h
)



SET(DIALOGS_SRCS
changedialog.cpp
digisettingsdialog.cpp
savedialog.cpp labeldialog.cpp
searchdialog.cpp     settingsdialog.cpp
newprojdialog.cpp
exportdialog.cpp loaddialog.cpp
)

SET(DIALOGS_MOC_HDRS
changedialog.h
digisettingsdialog.h
exportdialog.h
labeldialog.h
loaddialog.h
newprojdialog.h
savedialog.h
searchdialog.h
settingsdialog.h
)

SET(DIALOGS_UIC_HDRS
searchdialog.ui
)

QT4_WRAP_CPP( DIALOGS_MOC_SRCS ${DIALOGS_MOC_HDRS} )

QT4_WRAP_UI( DIALOGS_UIC_SRCS ${DIALOGS_UIC_HDRS} )

ADD_LIBRARY(dialogs ${DIALOGS_HDRS} ${DIALOGS_SRCS} ${DIALOGS_MOC_SRCS} ${DIALOGS_UIC_SRCS})
