# To have CMake pick a Qt installation of your choice that won't be found
# automatically, set the CMAKE_PREFIX_PATH environment variable. For example:
# "export CMAKE_PREFIX_PATH=/usr/local/trolltech/qt4.3.5"
#
# You can change the install location by running cmake like this: mkdir build;
# cd build cmake .. -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#
# By default, the prefix is "/usr/local"
#
# Tests are enabled by default. To run the tests, try one of these: make test
# env CTEST_OUTPUT_ON_FAILURE=1 make test    # for verbose output

project(qucs_lib CXX)
cmake_minimum_required(VERSION 3.16)

set(coreSchematicPath ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

# use top VERSION file
file(STRINGS ${coreSchematicPath}/VERSION QUCS_VERSION)

set(PROJECT_VERSION "${QUCS_VERSION}")

# add compiler definitions/symbols
# already in qucs_app CMakeLists.txt. Maybe renaming it?
#add_compile_definitions(PACKAGE_VERSION="${PROJECT_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2020")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")


# Only do these if this is the main project, and not if it is included through
# add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# enable testing for current directory and below
#enable_testing()

## ------ WHY needed?
# If Git hash not defined, try to define it
#if(NOT GIT)
#  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../.git)
#    find_package(Git)
#    # Get the latest abbreviated commit hash of the working branch
#    execute_process(
#      COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1u
#      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#      OUTPUT_VARIABLE GIT_COMMIT_HASH)
#    set(GIT ${GIT_COMMIT_HASH})
#    message(STATUS "Found Git repository, last commit hash: ${GIT}")
#  endif()
#endif()

#if(UNIX AND NOT APPLE)
#  # string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE) set(BIN_INSTALL_DIR
#  # "bin") set(DOC_INSTALL_DIR "share/doc/${PROJECT_NAME_LOWERCASE}/")
#else()
#  # set(BIN_INSTALL_DIR ".") set(DOC_INSTALL_DIR ".")
#endif()

#
#add_definitions(-DHAVE_CONFIG_H)

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

## configure the header config.h
#configure_file("${PROJECT_SOURCE_DIR}/../config.h.cmake"
#               "${PROJECT_BINARY_DIR}/config.h")

#include_directories("${PROJECT_BINARY_DIR}")
#include_directories(${CMAKE_SOURCE_DIR})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") # enable warning level
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ") # enable C++11

## flag not available in mingw 4.8.2, MSVC10
#if(NOT WIN32)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register ")
#endif()

## For some reason, on Windows QT_LIBRARIES point to static(?) .a libs which do
## not provide the needed symbols. They are found on the .dll libraries.
#if(WIN32)
#  link_directories("${QT_BINARY_DIR}")
#endif(WIN32)

#
# Set position independed code PIC
#
#if(UNIX AND NOT APPLE)
#  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
#    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#  endif()
#endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/readMakeList.txt)

add_subdirectory(${coreSchematicPath}/bitmaps src/bitmaps) #-> added as resources
add_subdirectory(${coreSchematicPath}/octave src/octave)
add_subdirectory(${coreSchematicPath}/python src/python)
#add_subdirectory(${coreSchematicPath}/tests src/tests)

readVariable(${coreSchematicPath}/MakeList CPP_SRC QUCS_SRCS)
readVariable(${coreSchematicPath}/MakeList CPP_HEADERS QUCS_HDRS)
readVariable(${coreSchematicPath}/MakeList UI_FILES UIC_HDRS)
readVariable(${coreSchematicPath}/MakeList DIALOG_SRC QUCS_DIALOG_SRC)
list(TRANSFORM QUCS_SRCS PREPEND ${coreSchematicPath}/)
list(TRANSFORM QUCS_HDRS PREPEND ${coreSchematicPath}/)
list(TRANSFORM UIC_HDRS PREPEND ${coreSchematicPath}/)
list(TRANSFORM QUCS_DIALOG_SRC PREPEND ${coreSchematicPath}/)

#
# Read variable to generate rules for building source files from bitmap resources
#
readVariable(${coreSchematicPath}/MakeList RESOURCES QUCS_RESOURCES)
list(TRANSFORM QUCS_RESOURCES PREPEND ${coreSchematicPath}/)

##
## configure Apple bundle information
##
#if(APPLE)
#  # set information on Info.plist file
#  set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
#  set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
#  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
#  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
#  set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
#  set(MACOSX_BUNDLE_GUI_IDENTIFIER
#      "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
#  set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
#  set(MACOSX_BUNDLE_ICON_FILE qucs.icns)

#  # set where in the bundle to put the icns file
#  set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/bitmaps/qucs.icns
#                              PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
#  # include the icns file in the target
#  set(QUCS_SRCS ${QUCS_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/bitmaps/qucs.icns)

#  # This tells cmake where to place the translations inside the bundle
#  # SET_SOURCE_FILES_PROPERTIES( ${LANG_SRCS} PROPERTIES MACOSX_PACKAGE_LOCATION
#  # Resources/lang ) include the translation files in the target
#  # SET(RESOURCES_SRCS ${RESOURCES_SRCS} ${LANG_SRCS})
#endif(APPLE)

##
## Set up RPATH for the project
##
#option(ENABLE_RPATH "Enable rpath support on Linux and Mac" ON)
#if(NOT CMAKE_INSTALL_RPATH)
#  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#endif()
#if(APPLE AND NOT CMAKE_INSTALL_NAME_DIR)
#  set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
#endif()
#if(UNIX AND ENABLE_RPATH)
#  set(CMAKE_SKIP_BUILD_RPATH FALSE)
#  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#  set(CMAKE_MACOSX_RPATH TRUE)
#endif()

## schematic library
add_library(coreSchematic OBJECT ${QUCS_SRCS} ${QUCS_RESOURCES} ${UIC_HDRS} ${QUCS_DIALOG_SRC})
target_include_directories(coreSchematic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_include_directories(coreSchematic PRIVATE ${coreSchematicPath})

target_link_libraries(coreSchematic Qt5::Core)
target_link_libraries(coreSchematic Qt5::Gui)
target_link_libraries(coreSchematic Qt5::Widgets)
target_link_libraries(coreSchematic Qt5::Svg)
target_link_libraries(coreSchematic Qt5::PrintSupport)
target_link_libraries(coreSchematic Qt5::Script)

## create shared library
add_library(
  qucsschematic SHARED
  $<TARGET_OBJECTS:coreSchematic> #$<TARGET_OBJECTS:components>
  )

