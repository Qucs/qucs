# This file is part of Qucs.
# Create gui library

# (C) 2021 Guilherme Brondani Torri

# TODO get rid of these relative paths
include(../../readMakeList.txt)

set(pluginPath ${CMAKE_CURRENT_SOURCE_DIR}/../../../plugins/gui)

# TODO check uniformity of list names on MakeList

# Hack on Makelist breaks the macro

# BUG mixup of .cc (conver to .h) and .cpp makes it impossible to find the file...

# TODO some files are failing to compile
#readVariable(${pluginPath}/MakeList QT_PLUGIN_SRC SRCS)
readVariable(${pluginPath}/MakeList QT_PLUGIN_SRC_CMAKE SRCS)

list(TRANSFORM SRCS PREPEND ${pluginPath}/)

set(PLUGIN gui)

add_library(${PLUGIN} SHARED ${SRCS} ${HDR})

#target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../legacy/components)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
#target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../../) # pick generated config.h

target_link_libraries(${PLUGIN} Qt5::Widgets)
target_link_libraries(${PLUGIN} Qt5::Core)
target_link_libraries(${PLUGIN} Qt5::Gui)

# drop lib from lib[*].so
set_target_properties(${PLUGIN} PROPERTIES PREFIX "")

# See attach_default_plugins() for expected paths
# Should go into /plugins/gui.so
# TODO figure out way to copy plugin into expected QUCS_PLUGPATH
add_custom_command(TARGET ${PLUGIN} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
     "$<TARGET_FILE:${PLUGIN}>"
     "${CMAKE_CURRENT_BINARY_DIR}/../$<TARGET_FILE_NAME:${PLUGIN}>"
     COMMENT "Copying to plugin directory")
