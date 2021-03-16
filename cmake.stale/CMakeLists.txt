# this file is part of Qucs.

# (C) 2014 Guilherme Brondani Torri
#     2020 Martin Marmsoler
#     2020 Felix Salfelder
# License: GPLv3+

# TODO
# - move all the Qt configuration to this top level configuration file
# - run cmake format
# - the MakeList files need to be added as dependency to trigger re-build if changed

project(qucs CXX)
cmake_minimum_required(VERSION 3.18)
cmake_policy(VERSION 3.18)

# Set standard installation paths.
# TODO: check the difference.
include(GNUInstallDirs)


# TODO: this is outdated.
# Why needed? Might be useful to tag the binary with git version
# If Git hash not defined, try to define it
set(GIT unknown)
if(EXISTS ${CMAKE_SOURCE_DIR}/../.git)
  find_package(Git)
  # Get the latest abbreviated commit hash of the working branch
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1u
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH)
  set(GIT ${GIT_COMMIT_HASH})
  message(STATUS "Found Git repository, last commit hash: ${GIT}")
endif()


# Set up project strings and variables
set(QUCS_URL "https://sf.net/p/qucs")
set(QUCS_BUGREPORT "qucs-bugs@lists.sourceforge.net")

message(STATUS "${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}
    ${qucs-suite_BINARY_DIR}")

# use top VERSION file
file(STRINGS ${PROJECT_SOURCE_DIR}/../VERSION QUCS_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${QUCS_VERSION}")

set(PROJECT_VERSION "${QUCS_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2021")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

# Only do these if this is the main project, and not if it is included through
# add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# when true, all the dependencies (Qucsator, legacy and so on are also compiled)
#option(CompileDependencies "Compile dependencies related to qucs (qucsator ...), default ON" ON)

# add compiler definitions/symbols
add_compile_definitions(QUCS_PREFIX="${prefix}")
#add_compile_definitions(QUCS_PLUGPATH="${pkglibdir}")

# crash if not defined
# figure out a way to put the plugin.so files on the PLUGPATH, both during build and after install
add_compile_definitions(QUCS_PLUGPATH="${CMAKE_BINARY_DIR}/")

# also defined into config.h ?
# TODO either use -D or the header ?
#set(PACKAGE_VERSION "${PROJECT_VERSION}")
#add_compile_definitions(PACKAGE_VERSION="${PROJECT_VERSION}")

# enable testing for current directory and below
#enable_testing()

find_package(
  Qt5
  REQUIRED
  Core
  Widgets
  Gui
  Svg
  Xml
  Script
  Test
  PrintSupport
  )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_DEBUG TRUE)

add_definitions(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS)

configure_file("${CMAKE_SOURCE_DIR}/config.h.cmake"
               "${CMAKE_BINARY_DIR}/include/config.h")

add_definitions(-DHAVE_CONFIG_H)

# set qt_compat.h
add_definitions(-DQT_MAJOR_VERSION=5)


# Process CMakeLists.txt relative to this source file
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/main)
#add_subdirectory(${CMAKE_SOURCE_DIR}/bitmaps) no need, added from qrc file

# add_subdirectory(man)
# add_subdirectory(plugins)
add_subdirectory(legacy)
add_subdirectory(legacy/action)
add_subdirectory(legacy/components)
add_subdirectory(legacy/paintings)
add_subdirectory(legacy/qucsator)

add_subdirectory(plugins/gui)
add_subdirectory(plugins/misc)

# add_subdirectory(qucs-activefilter)
# add_subdirectory(qucs-attenuator)
# # ADD_SUBDIRECTORY( qucs-doc )
# add_subdirectory(qucs-filter)
# add_subdirectory(qucs-lib)
# add_subdirectory(qucs-transcalc)
# add_subdirectory(qucs-rescodes)
# # ADD_SUBDIRECTORY( examples )
# add_subdirectory(translations)
# add_subdirectory(contrib)
# add_subdirectory(qucs-powercombining)

#
# Custom uninstall target
#
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
#               "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake" IMMEDIATE @ONLY)

#add_custom_target(uninstall-ui COMMAND ${CMAKE_COMMAND} -P
#                                    ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)
