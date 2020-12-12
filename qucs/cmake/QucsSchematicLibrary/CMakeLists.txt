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
add_compile_definitions(PACKAGE_VERSION="${PROJECT_VERSION}")
add_compile_definitions(QUCS_PREFIX="${prefix}")
add_compile_definitions(QUCS_PLUGPATH="${pkglibdir}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2014")
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

#  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    set(QT_LIBRARIES QtCored4 QtGuid4 Qt3Supportd4 QtSvgd4 QtXmld4 QtScriptd4)
#  else()
#    set(QT_LIBRARIES QtCore4 QtGui4 Qt3Support4 QtSvg4 QtXml4 QtScript4)
#  endif()
#endif(WIN32)

#
# Set position independed code PIC
#
#if(UNIX AND NOT APPLE)
#  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
#    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#  endif()
#endif()

#add_definitions(${QT_DEFINITIONS} -DQT3_SUPPORT_WARNINGS)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/readMakeList.txt)

add_subdirectory(${coreSchematicPath}/bitmaps src/bitmaps) #-> added as resources
add_subdirectory(${coreSchematicPath}/octave src/octave)
add_subdirectory(${coreSchematicPath}/python src/python)
#add_subdirectory(${coreSchematicPath}/tests src/tests)

readVariable(${coreSchematicPath}/MakeList CPP_SRC QUCS_SRCS)
readVariable(${coreSchematicPath}/MakeList CPP_HEADERS QUCS_HDRS)
readVariable(${coreSchematicPath}/MakeList UI_FILES UIC_HDRS)
list(TRANSFORM QUCS_SRCS PREPEND ${coreSchematicPath}/)
list(TRANSFORM QUCS_HDRS PREPEND ${coreSchematicPath}/)
list(TRANSFORM UIC_HDRS PREPEND ${coreSchematicPath}/)

#
# Read variable to generate rules for building source files from bitmap resources
#
readVariable(${coreSchematicPath}/MakeList RESOURCES QUCS_RESOURCES)
list(TRANSFORM QUCS_RESOURCES PREPEND ${coreSchematicPath}/)

#if(UNIX AND NOT APPLE)

#  set(ICON16 bitmaps/hicolor/16x16/apps/qucs.png)
#  set(ICON22 bitmaps/hicolor/22x22/apps/qucs.png)
#  set(ICON32 bitmaps/hicolor/32x32/apps/qucs.png)
#  set(ICON48 bitmaps/hicolor/48x48/apps/qucs.png)
#  set(ICON64 bitmaps/hicolor/64x64/apps/qucs.png)
#  set(ICON128 bitmaps/hicolor/128x128/apps/qucs.png)
#  set(ICON256 bitmaps/hicolor/256x256/apps/qucs.png)
#  set(ICON512 bitmaps/hicolor/512x512/apps/qucs.png)
#  set(ICONsc bitmaps/hicolor/scalable/apps/qucs.svg)

#  install(FILES ${ICON16}
#          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/16x16/apps)
#  install(FILES ${ICON22}
#          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/22x22/apps)
#  install(FILES ${ICON32}
#          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/apps)
#  install(FILES ${ICON48}
#          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps)
#  install(FILES ${ICON64}
#          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/64x64/apps)
#  install(FILES ${ICON128}
#          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/128x128/apps)
#  install(FILES ${ICON256}
#          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/256x256/apps)
#  install(FILES ${ICON512}
#          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/512x512/apps)
#  install(FILES ${ICONsc}
#          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)
#endif()

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
add_library(coreSchematic OBJECT ${QUCS_SRCS} ${QUCS_RESOURCES} ${UIC_HDRS})
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

## link system libraries
#target_link_libraries(qucsschematic ${CMAKE_DL_LIBS})

