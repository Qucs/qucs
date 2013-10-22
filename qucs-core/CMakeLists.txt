# You can change the install location by
# running cmake like this:
#   mkdir build; cd build
#   cmake .. -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#
# By default, the prefix is "/usr/local"

PROJECT(qucs-core CXX)

cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)

#set(CMAKE_VERBOSE_MAKEFILE ON)

OPTION(ENABLE_QUCSLIB "enable qucslib build, default: OFF")
#OPTION(ENABLE_DOUBLE " type of double representation, default=double")
#-D <var>:<type>=<value>: Create a cmake cache entry.


#  AC_DEFINE(DEBUG, 1, [Define if debug output should be supported.])
#else
#  AC_DEFINE(NDEBUG, 1, [Define if debug code should be suppressed.])
#  if test "x$GCC" = xyes; then
#    CFLAGS="`echo $CFLAGS | sed -e 's/\-g //g'`"
#    CXXFLAGS="`echo $CXXFLAGS | sed -e 's/\-g //g'`"
# disable stripping (debian bug #437876)
#    case $host_os in
#      *darwin*) LDFLAGS="$LDFLAGS" ;;
#      *)        LDFLAGS="$LDFLAGS -s" ;;
#    esac
#  fi




set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "0")
set(PROJECT_VERSION_PATCH "18")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2013")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

#TODO rename the above variables?

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




# defines nr_double_t
#ADD_DEFINITIONS( -DHAVE_CONFIG_H )

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/math

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ../../
                     ../math
                     ../ )



# What about admsXml?? build or used packaged?
# There is no admsXml packaged out there... need to build if not installed

#SUBDIRS += src doc .

#ACLOCAL_AMFLAGS = -I m4

ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( doc )

#EXTRA_DIST = BUGS autogen.sh depcomp RELEASE
