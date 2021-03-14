# This file is part of Qucs.
# Create components library

# (C) 2021 Guilherme Brondani Torri

include(../../readMakeList.txt)

set(compPath ${CMAKE_CURRENT_SOURCE_DIR}/../../../legacy/components)

# TODO add other source objects
# spicefile.h is failing due to missing headers?

# Srcs
readVariable(${compPath}/MakeList DEFAULT_COMPONENTS DEFAULT_)
readVariable(${compPath}/MakeList LUMPED_COMPONENTS LUMPED_)
readVariable(${compPath}/MakeList RF_COMPONENTS RF_)
readVariable(${compPath}/MakeList TASK_ELEMENTS TASK_)
##readVariable(${compPath}/MakeList NOTYET_SIM ?_)
##readVariable(${compPath}/MakeList LOGIC_COMPONENTS ?_)
readVariable(${compPath}/MakeList OTHERCOMPONENTS OTHER_)

list(TRANSFORM DEFAULT_ PREPEND ${compPath}/)
list(TRANSFORM LUMPED_ PREPEND ${compPath}/)
list(TRANSFORM RF_ PREPEND ${compPath}/)
list(TRANSFORM TASK_ PREPEND ${compPath}/)
list(TRANSFORM OTHER_ PREPEND ${compPath}/)

set(PLUGIN components)

# Headers
#readVariable(${compPath}/MakeList HEADERS_ HEADERS_)
#list(TRANSFORM HEADERS_ PREPEND ${compPath}/)

#readVariable(${compPath}/MakeList MOC MOC_)
#list(TRANSFORM MOC_ PREPEND ${compPath}/)

#set(COMPONENTS_MOC_HDRS spicedialog.h optimizedialog.h
#                        spicefile.h)

# An object library compiles source files but does not archive or link their object files into a library.
#add_library(components OBJECT ${HEADERS_} ${COMPONENTS_SRCS}
#                              ${COMPONENTS_MOC_SRCS})


#set(COMPONENTS_MOC_HDRS componentdialog.h spicedialog.h optimizedialog.h
#                        spicefile.h)
#
#    QT5_WRAP_CPP(COMPONENTS_MOC_SRCS ${COMPONENTS_MOC_HDRS})
#
#

#QT5_WRAP_CPP(COMPONENTS_MOC_SRCS ${MOC_})

add_library(${PLUGIN} SHARED
  ${DEFAULT_}
  ${LUMPED_}
  ${RF_}
  ${TASK_}
  ${OTHER_}
  #${COMPONENTS_MOC_SRCS})
  #${MOC_})
  )

# BUG there are 2 headers with the same name:
#./legacy/components/component.h
#./include/component.h
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../legacy/components)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include) # for node?
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../)

target_link_libraries(${PLUGIN} Qt5::Widgets)
target_link_libraries(${PLUGIN} Qt5::Core)
target_link_libraries(${PLUGIN} Qt5::Gui)
target_link_libraries(${PLUGIN} Qt5::Script)

# drop lib from lib[*].so
set_target_properties(${PLUGIN} PROPERTIES PREFIX "")

# See attach_default_plugins() for expected paths
# Should go into /legacy/components.so
# TODO figure out way to copy plugin into expected QUCS_PLUGPATH
add_custom_command(TARGET ${PLUGIN} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
     "$<TARGET_FILE:${PLUGIN}>"
     "${CMAKE_CURRENT_BINARY_DIR}/../$<TARGET_FILE_NAME:${PLUGIN}>"
     COMMENT "Copying to plugin directory")
