# this file is part of Qucs.

# (C) 2021 Guilherme Brondani Torri

# TODO get rid of these relative paths
include(../../readMakeList.txt)

set(pluginPath ${CMAKE_CURRENT_SOURCE_DIR}/../../../plugins/gui)

# TODO check uniformity of list names on MakeList
# Hack on Makelist breaks the macro
# mixup of .cc (conver to .h) and .cpp makes it impossible to find the file...
#readVariable(${pluginPath}/MakeList QT_PLUGIN_SRC SRCS)
readVariable(${pluginPath}/MakeList QT_PLUGIN_SRC_CMAKE SRCS)

list(TRANSFORM SRCS PREPEND ${pluginPath}/)

add_library(gui SHARED ${SRCS} ${HDR})

# why again? move to top most cmakelists?
find_package(
  Qt5
  REQUIRED
  Widgets
  Core
  Gui
  )

add_definitions(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS)

target_include_directories(gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
target_include_directories(gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
target_include_directories(gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../)

target_link_libraries(gui Qt5::Widgets)
target_link_libraries(gui Qt5::Core)
target_link_libraries(gui Qt5::Gui)

# drop lib from lib[*].so
set_target_properties(gui PROPERTIES PREFIX "")
