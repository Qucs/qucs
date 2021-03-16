# This file is part of Qucs.
# Create legacy library

# (C) 2021 Felix Salfelder
#     2021 Guilherme Brondani Torri

include(../readMakeList.txt)

set(legacyPath ${CMAKE_CURRENT_SOURCE_DIR}/../../legacy)

readVariable(${legacyPath}/MakeList LEGACY_SRC SRCS)
readVariable(${legacyPath}/MakeList HDR HDR)

list(TRANSFORM SRCS PREPEND ${legacyPath}/)
list(TRANSFORM HDR PREPEND ${legacyPath}/)

add_library(legacy SHARED ${SRCS} ${HDR})

# todo: use common CMakeList
set(PLUGIN legacy)

target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../include)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../)

target_link_libraries(${PLUGIN} Qt5::Core)
target_link_libraries(${PLUGIN} Qt5::Gui)
target_link_libraries(${PLUGIN} Qt5::Widgets)

# drop lib from lib*.so
set_target_properties(${PLUGIN} PROPERTIES PREFIX "")

# See attach_default_plugins() for expected paths
# Should go into /${PLUGIN}.so
# TODO figure out way to copy plugin into expected QUCS_PLUGPATH
add_custom_command(TARGET ${PLUGIN} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
     "$<TARGET_FILE:${PLUGIN}>"
     "${PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:${PLUGIN}>"
     COMMENT "Copying to plugin directory")
