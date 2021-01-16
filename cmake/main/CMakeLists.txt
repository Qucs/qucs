# Qucs application creation
# (C) 2014 Guilherme Brondani Torri
#     2020 Martin Marmsoler
#     2020 Felix Salfelder


project(qucs_gui)
cmake_minimum_required(VERSION 3.16)

# use top VERSION file
file(STRINGS ${PROJECT_SOURCE_DIR}/../../VERSION PROJECT_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${PROJECT_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2020")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

# when true, all the dependencies (Qucsator, legacy and so on are also compiled)
option(CompileDependencies "Compile dependencies related to qucs (qucsator ...), default ON" ON)
option(EnableTracing "If enabled, all traceX macros output a message, default OFF" OFF)

# add compiler definitions/symbols
add_compile_definitions(QUCS_PREFIX="${prefix}")
add_compile_definitions(QUCS_PLUGPATH="${pkglibdir}")
if (EnableTracing)
	add_compile_definitions(DO_TRACE)
endif()

# add dependencies
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../legacy legacy)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/qucsator legacy/qucsator)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/components legacy/components)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/paintings legacy/paintings)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/libfiles legacy/libfiles)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../plugins/misc plugins/misc)

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

# configure the header config.h
configure_file("${PROJECT_SOURCE_DIR}/../../config.h.cmake"
               "${PROJECT_BINARY_DIR}/config.h")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") # enable warning level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ") # enable C++11

# flag not available in mingw 4.8.2, MSVC10
if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register ")
endif()

## For some reason, on Windows QT_LIBRARIES point to static(?) .a libs which do
## not provide the needed symbols. They are found on the .dll libraries.
#if(WIN32)
#  link_directories("${QT_BINARY_DIR}")
#endif(WIN32)

#
# Set position independed code PIC
#
if(UNIX AND NOT APPLE)
  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()
endif()

find_package(
  Qt5
  REQUIRED
  Widgets
  Svg
  PrintSupport
  )

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

##
## CMake's way of creating an executable
##
add_executable(qucs MACOSX_BUNDLE WIN32 ${CMAKE_CURRENT_SOURCE_DIR}/../../main/main.cpp)
target_link_libraries(qucs libqucs)
target_link_libraries(qucs Qt5::Widgets)
target_link_libraries(qucs Qt5::Svg)
# otherwise I'm getting /usr/bin/ld: QucsSchematicLibrary/libqucsschematic.so: undefined reference to `QPrinter::QPrinter(QPrinter::PrinterMode)'
# why do I need the printersupport?
#target_link_libraries(qucs Qt5::PrintSupport)

# Don't like to include them all here. TODO: find cleaner method. Included in qucsschematic already. So can it be reused?
target_include_directories(qucs PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include
 ${CMAKE_CURRENT_SOURCE_DIR}/../../src
 ${CMAKE_CURRENT_BINARY_DIR}
 )

include(compileAndInstallPlugins.cmake)

# install Qucs application
include(${CMAKE_CURRENT_SOURCE_DIR}/installQucsApplication.cmake)

