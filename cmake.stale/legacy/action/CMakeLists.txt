# this file is part of Qucs.

# (C) 2021 Guilherme Brondani Torri

include(../../readMakeList.txt)

set(legacyPath ${CMAKE_CURRENT_SOURCE_DIR}/../../../legacy/action)

readVariable(${legacyPath}/MakeList PLUGIN_SRC SRCS)

list(TRANSFORM SRCS PREPEND ${legacyPath}/)

add_library(action SHARED ${SRCS} ${HDR})

# why again? move to top most cmakelists?
find_package(
  Qt5
  REQUIRED
  Widgets
  Core
  Gui
  )

add_definitions(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS)

target_include_directories(action PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
target_include_directories(action PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
target_include_directories(action PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../)

target_link_libraries(action Qt5::Widgets)
target_link_libraries(action Qt5::Core)
target_link_libraries(action Qt5::Gui)

# drop lib from lib[*].so
set_target_properties(action PROPERTIES PREFIX "")
