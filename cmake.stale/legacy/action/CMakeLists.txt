# This file is part of Qucs.
# Create actions library

# (C) 2021 Guilherme Brondani Torri

include(../../readMakeList.txt)

set(legacyPath ${CMAKE_CURRENT_SOURCE_DIR}/../../../legacy/action)

readVariable(${legacyPath}/MakeList PLUGIN_SRC SRCS)

list(TRANSFORM SRCS PREPEND ${legacyPath}/)

set(PLUGIN action)

add_library(${PLUGIN} SHARED ${SRCS} ${HDR})

target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../)

target_link_libraries(${PLUGIN} Qt5::Widgets)
target_link_libraries(${PLUGIN} Qt5::Core)
target_link_libraries(${PLUGIN} Qt5::Gui)

# drop lib from lib[*].so
set_target_properties(${PLUGIN} PROPERTIES PREFIX "")

# See attach_default_plugins() for expected paths
# Should go into /legacy/action.so
# TODO figure out way to copy plugin into expected QUCS_PLUGPATH
add_custom_command(TARGET ${PLUGIN} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
     "$<TARGET_FILE:${PLUGIN}>"
     "${CMAKE_CURRENT_BINARY_DIR}/../$<TARGET_FILE_NAME:${PLUGIN}>"
     COMMENT "Copying to plugin directory")
