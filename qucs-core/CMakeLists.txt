# You can change the install location by
# running cmake like this:
#   mkdir build; cd build
#   cmake .. -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#
# By default, the prefix is "/usr/local"
## DEPENDS
# adms
# sed, gperf, make

PROJECT(qucs-core CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#set(CMAKE_VERBOSE_MAKEFILE ON)

# otherwise qucsator cannot generate qucsdefs.h
add_definitions(-DDEBUG)

# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

# TODO
#OPTION(ENABLE_QUCSLIB "enable qucslib build, default: OFF")
#OPTION(ENABLE_DOUBLE " type of double representation, default=double")

# TODO configure debug/release flags


# Flags used on Autotools:
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
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "0")
set(PROJECT_VERSION_PATCH "18")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2013")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

#TODO rename the above variables? Project/Package?

# Define to the address where bug reports for this package should be sent.
set(PACKAGE_BUGREPORT "qucs-bugs@lists.sourceforge.net")

# Define to the full name of this package.
set(PACKAGE_NAME "qucs-core")

# Define to the full name and version of this package.
set(PACKAGE_STRING "${PACKAGE_NAME} ${PROJECT_VERSION}")

# Define to the one symbol short name of this package.
set(PACKAGE_TARNAME ${PACKAGE_NAME})

# Define to the home page for this package.
set(PACKAGE_URL "http://sourceforge.net/projects/qucs/")

# Define to the version of this package.
set(PACKAGE_VERSION ${PROJECT_VERSION})

#
# Avoid source tree pollution
#
If(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "In-source builds are not permitted. Make a separate folder for building:\nmkdir build; cd build; cmake ..\nBefore that, remove the files already created:\nrm -rf CMakeCache.txt CMakeFiles")
endif(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")


INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/math )

# What about admsXml?? build or used packaged?
# There is no admsXml packaged out there... need to build if not installed

#ACLOCAL_AMFLAGS = -I m4

ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( doc )

#EXTRA_DIST = BUGS autogen.sh depcomp RELEASE

# TODO tarball
# TODO bundle

SET(CPACK_GENERATOR "TGZ")
#SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "guitorri") #required

# build a CPack driven installer package
#include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VERSION_MAJOR 0)
set (CPACK_PACKAGE_VERSION_MINOR 18)
include (CPack)
