##
# You can change the install location by
# running cmake like this:
#   mkdir build; cd build
#   cmake .. -DCMAKE_INSTALL_PREFIX=/new/install/prefix

## Notes
# * By default, the prefix is "/usr/local"
# * Use -DADMSXML_DIR=[path] to give the path containing admsXml

## Dependencies
# * CMake 2.8.8
# * adms 2.3.0
# * sed
# * gperf
# * flex
# * bison
# * make
# * C++ compiler


##
# qucs-core
#   * qucsator, libqucs
#   * qucsconv
#
PROJECT(qucs-core CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

#SET(CMAKE_VERBOSE_MAKEFILE ON)

# otherwise qucsator cannot generate qucsdefs.h
ADD_DEFINITIONS(-DDEBUG)

# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

# TODO
#OPTION(ENABLE_QUCSLIB "enable qucslib build, default: OFF")
#OPTION(ENABLE_DOUBLE " type of double representation, default=double")

# TODO configure debug/release flags


# TODO check flags used on Autotools are needed:
#
# -pipe : Use pipes rather than temporary files for communication between the various stages of compilation.
#
# https://blog.mozilla.org/nnethercote/2011/01/18/the-dangers-of-fno-exceptions/
# -fno-exceptions option is used, which means that exception-handling is disabled.
#
# -fno-rtti :  Disable generation of information about every class with virtual functions.
#
# -fno-check-new : specific to GCC
#
# -Wmissing-prototypes : Warn if a global function is defined without a previous prototype declaration.
#
#
SET(PROJECT_VERSION_MAJOR "0")
SET(PROJECT_VERSION_MINOR "0")
SET(PROJECT_VERSION_PATCH "18")
SET(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
SET(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
SET(PROJECT_COPYRIGHT_YEAR "2013")
SET(PROJECT_DOMAIN_FIRST "qucs")
SET(PROJECT_DOMAIN_SECOND "org")

#TODO rename the above variables? Project/Package?

# Define to the address where bug reports for this package should be sent.
SET(PACKAGE_BUGREPORT "qucs-bugs@lists.sourceforge.net")

# Define to the full name of this package.
SET(PACKAGE_NAME "qucs-core")

# Define to the full name and version of this package.
SET(PACKAGE_STRING "${PACKAGE_NAME} ${PROJECT_VERSION}")

# Define to the one symbol short name of this package.
SET(PACKAGE_TARNAME ${PACKAGE_NAME})

# Define to the home page for this package.
SET(PACKAGE_URL "http://sourceforge.net/projects/qucs/")

# Define to the version of this package.
SET(PACKAGE_VERSION ${PROJECT_VERSION})

#
# Avoid source tree pollution
#
IF(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	MESSAGE(FATAL_ERROR "\nIn-source builds are not permitted.
                       Make a separate folder for building:
                       $ mkdir build; cd build; cmake ..
                       Before that, remove the files already created:
                       $ rm -rf CMakeCache.txt CMakeFiles")
ENDIF()

#
# Set locations of CMake modules, used on tests, find,...
#
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")


#
# Need Flex / Bison
#
SET(FLEX_MINIMAL_VERSION "2.5.9")
FIND_PACKAGE(FLEX REQUIRED)
IF(FLEX_FOUND)
  MESSAGE(STATUS "Found flex: ${FLEX_EXECUTABLE} / Version: ${FLEX_VERSION}" )
ENDIF()

SET(BISON_MINIMAL_VERSION "2.4")
FIND_PACKAGE(BISON REQUIRED)
IF(BISON_FOUND)
  MESSAGE(STATUS "Found bison: ${BISON_EXECUTABLE} / Version: ${BISON_VERSION}" )
ENDIF()

#
# Check for sed
#
FIND_PROGRAM(SED_TOOL NAMES sed)
IF(NOT SED_TOOL)
	MESSAGE(FATAL_ERROR "Unable to find sed")
ELSE()
  MESSAGE(STATUS "Found sed: " ${SED_TOOL})
ENDIF()

#
# Check for gperf
#
FIND_PROGRAM(GPERF_TOOL NAMES gperf)
IF (NOT GPERF_TOOL)
	MESSAGE(FATAL_ERROR "Unable to find gperf")
ELSE()
  MESSAGE(STATUS "Found gperf: " ${GPERF_TOOL})
ENDIF()

#
# Check if admsXml is available
#  * Use -DADMSXML_DIR=[path] to give the path containing admsXml
#  * Try a few othe locations
#
FIND_PROGRAM( ADMSXML admsXml
  HINTS ${ADMSXML_DIR}
  PATHS /usr/local/bin/ /opt/local/bin/
  DOC "admsXml application" )

IF(ADMSXML)
  MESSAGE(STATUS "Found admsXml: " ${ADMSXML})
ELSE()
  MESSAGE(FATAL_ERROR "admsXml was not found!")
ENDIF()

#
# Set up RPATH for the project
#
option(ENABLE_RPATH "Enable rpath support on Linux and Mac" ON)
if(NOT CMAKE_INSTALL_RPATH)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()
if(APPLE AND NOT CMAKE_INSTALL_NAME_DIR)
  set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
endif()
if(UNIX AND ENABLE_RPATH)
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

#
# Go look for stuff to build/install...
#
ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( doc )


#
# Custom uninstall target
#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)



# TODO install distributables
#EXTRA_DIST = BUGS autogen.sh depcomp RELEASE

# TODO tarball
# TODO bundle

SET(CPACK_GENERATOR "TGZ")
#SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "guitorri") #required

# build a CPack driven installer package
#include (InstallRequiredSystemLibraries)
SET (CPACK_PACKAGE_VERSION_MAJOR 0)
SET (CPACK_PACKAGE_VERSION_MINOR 18)
include (CPack)
