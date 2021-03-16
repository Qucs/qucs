# This file is part of Qucs.
# Create misc library

# (C) 2021 Guilherme Brondani Torri

# TODO get rid of these relative paths
include(../../readMakeList.txt)

set(pluginPath ${CMAKE_CURRENT_SOURCE_DIR}/../../../plugins/misc)

# TODO check uniformity of list names on MakeList
# Hack on Makelist breaks the macro
# mixup of .cc (conver to .h) and .cpp makes it impossible to find the file...
readVariable(${pluginPath}/MakeList RAW_SRCS SRCS)

list(TRANSFORM SRCS PREPEND ${pluginPath}/)

set(PLUGIN misc)

add_library(${PLUGIN} SHARED ${SRCS} ${HDR})

# TODO: define once?
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../../include)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../)

target_link_libraries(${PLUGIN} Qt5::Widgets)
target_link_libraries(${PLUGIN} Qt5::Core)
target_link_libraries(${PLUGIN} Qt5::Gui)

# drop lib from lib[*].so
set_target_properties(${PLUGIN} PROPERTIES PREFIX "")

# See attach_default_plugins() for expected paths
# Should go into /plugins/misc.so
# TODO figure out way to copy plugin into expected QUCS_PLUGPATH
add_custom_command(TARGET ${PLUGIN} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
     "$<TARGET_FILE:${PLUGIN}>"
     "${CMAKE_CURRENT_BINARY_DIR}/../$<TARGET_FILE_NAME:${PLUGIN}>"
     COMMENT "Copying to plugin directory")
