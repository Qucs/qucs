# this file is part of Qucs.

# (C) 2021 Felix Salfelder
#     2021 Guilherme Brondani Torri

include(../readMakeList.txt)

set(legacyPath ${CMAKE_CURRENT_SOURCE_DIR}/../../legacy)

readVariable(${legacyPath}/MakeList LEGACY_SRC SRCS)
readVariable(${legacyPath}/MakeList HDR HDR)

list(TRANSFORM SRCS PREPEND ${legacyPath}/)
list(TRANSFORM HDR PREPEND ${legacyPath}/)

add_definitions(-DHAVE_CONFIG_H)

add_library(legacy SHARED ${SRCS} ${HDR})

# why again? move to top most cmakelists?
find_package(
  Qt5
  REQUIRED
  Widgets
  Svg
  PrintSupport
  )

target_include_directories(legacy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_include_directories(legacy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
target_include_directories(legacy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../)

target_link_libraries(legacy Qt5::Core)
target_link_libraries(legacy Qt5::Gui)
target_link_libraries(legacy Qt5::Widgets)

# drop lib from liblegacy.so
set_target_properties(legacy PROPERTIES PREFIX "")

# TODO, somehow copy library into QUCS_PLUGPATH during build and relocate during install ?
