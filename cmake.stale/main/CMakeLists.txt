# This file is part of Qucs.
# Create Qucs application executable

# (C) 2014-2021 Guilherme Brondani Torri
#     2020 Martin Marmsoler
#     2020 Felix Salfelder
#     2021 Michał Walenciak

#
# Set up RPATH for the project
#
option(ENABLE_RPATH "Enable rpath support on Linux and Mac" ON)
if(NOT CMAKE_INSTALL_RPATH)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/qucs")
endif()
#if(APPLE AND NOT CMAKE_INSTALL_NAME_DIR)
#  set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
#endif()
if(UNIX AND ENABLE_RPATH)
  set(CMAKE_SKIP_BUILD_RPATH false)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH false)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true)
  set(CMAKE_MACOSX_RPATH true)
endif()

#
# Generate rules for building source files from bitmap resources
#
SET(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../../src/qucs_.qrc)
QT5_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})

##
## CMake's way of creating an executable
##
add_executable(qucs MACOSX_BUNDLE WIN32 ${CMAKE_CURRENT_SOURCE_DIR}/../../main/main.cpp ${RESOURCES_SRCS})

target_link_libraries(qucs qucsschematic)
target_link_libraries(qucs Qt5::Widgets)
target_link_libraries(qucs Qt5::Svg)
# otherwise I'm getting /usr/bin/ld: QucsSchematicLibrary/libqucsschematic.so: undefined reference to `QPrinter::QPrinter(QPrinter::PrinterMode)'
# why do I need the printersupport?
#target_link_libraries(qucs Qt5::PrintSupport)

target_include_directories(qucs PRIVATE
 ${CMAKE_CURRENT_BINARY_DIR}/../include
 )

# TODO: disentangle
# include(compileAndInstallDependencies.cmake)

# install additional files (icons etc.)
include(${CMAKE_CURRENT_SOURCE_DIR}/extra.cmake)
