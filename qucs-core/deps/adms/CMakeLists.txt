# skip CMake 3.0 project version management for the moment
if(POLICY CMP0048)
  cmake_policy(SET CMP0048 OLD)
endif()

#==================================
PROJECT(ADMS C)
cmake_minimum_required(VERSION 2.8)


option ( USE_MAINTAINER_MODE "Enable generation of code (parser source) from Perl scripts." OFF )

SET(PACKAGE_NAME "adms")
SET(PACKAGE_VERSION "2.3.3")
SET(PACKAGE_BUGREPORT "qucs-bugs@lists.sourceforge.net")

INCLUDE( CheckIncludeFile )
check_include_file( sys/stat.h HAVE_SYS_STAT_H )
check_include_file( locale.h HAVE_LOCALE_H)
check_include_file( float.h HAVE_FLOAT_H)
check_include_file( unistd.h HAVE_UNISTD_H)
check_include_file( stdlib.h HAVE_STDLIB_H)
check_include_file( string.h HAVE_STRING_H)

INCLUDE( CheckFunctionExists )
CHECK_FUNCTION_EXISTS (putenv HAVE_PUTENV)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Only care about Perl in case maintainer neededs to recreate souces
# * mkelements.pl
# * mktext.pl
# * mkgrammar.pl
if ( USE_MAINTAINER_MODE )
  # Checking for perl modules requires FindPerlModules.cmake
  # Check all required modules at once
  SET(CMAKE_MODULE_PATH "${ADMS_SOURCE_DIR}/cmake/modules")
  include(FindPerlModules)
  find_package(PerlModules COMPONENTS XML::LibXml GD)
  if(NOT PERLMODULES_FOUND)
    message(FATAL_ERROR "Not all required perl modules were found on your system")
  endif(NOT PERLMODULES_FOUND)
endif ( USE_MAINTAINER_MODE )


find_package(BISON)

# handle api.prefix, unused for the moment
MESSAGE("BISON_VERSION ${BISON_VERSION}")
IF (${BISON_VERSION} VERSION_LESS "2.6")
  SET (BISON_APIPREFIX "-p")
ELSE (${BISON_VERSION} VERSION_LESS "2.6")
  SET (BISON_APIPREFIX "-Dapi.prefix=")
ENDIF (${BISON_VERSION} VERSION_LESS "2.6")

find_package(FLEX)

ADD_SUBDIRECTORY(admsXml)

