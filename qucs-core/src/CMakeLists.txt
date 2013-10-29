
# TODO check wich checks do have inpact on the code, which HAVE_H change things

PROJECT(qucsator CXX C)

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/math  #precision.h
                     ${CMAKE_SOURCE_DIR}/src/components #component.h
                     ${CMAKE_SOURCE_DIR}/src/interface
                     # out of source include, generated stuff
                     ${CMAKE_BINARY_DIR} # /generated config.h
                     ${CMAKE_CURRENT_BINARY_DIR} #/src/gperfapphash.cpp
                     ${CMAKE_CURRENT_BINARY_DIR}/components #/verilog/*.core.h generated stuff
                      )

# Checks for libraries.
#AC_CHECK_LIB(m, sin) need to check for sin?
find_library(MATH_LIB NAMES m)
if(NOT MATH_LIB)
  message(SEND_ERROR "Math lib not found: ${MATH_LIB}" )
else()
  message(STATUS "Math lib found at: ${MATH_LIB}" )
endif()

# Checks for header files.
#AC_HEADER_STDC !! obsolete, need to check?
#Define STDC_HEADERS if the system has ANSI C header files.
#Specifically, this macro checks for `stdlib.h', `stdarg.h', `string.h', and `float.h';
# Lifted the cmake checks from gd-libdg, Lua
# https://bitbucket.org/libgd/gd-libgd
# https://github.com/LuaDist/libgd/tree/master/cmake/modules
INCLUDE (CheckIncludeFiles)
SET(CMAKE_REQUIRED_INCLUDES "/usr/include" "/usr/local/include")
SET(CMAKE_MODULE_PATH "${qucs-core_SOURCE_DIR}/cmake/modules")
include(AC_HEADER_STDC)
#include(CheckPrototypeExists)

# Further header checks
INCLUDE (CheckIncludeFile)
set(INCLUDES
  stddef.h
  stdlib.h
  string.h
  unistd.h
  ieeefp.h
  memory.h)

# Check if header can be included.
# Define HAVE_[basename]_H to 1 if you have the header.
foreach(header ${INCLUDES})
  get_filename_component(base ${header} NAME_WE)
  string(TOUPPER ${base} base)
  check_include_file ( ${header} HAVE_${base}_H )
  #message(STATUS "${header}  --> ${HAVE_${base}_H}")
endforeach()


# Checks for typedefs, structures, and compiler characteristics.
#AC_C_CONST  !!obsolete
#AC_C_CONST "This macro is
#obsolescent, as current C compilers support `const'.  New programs
#need not use this macro."
#/* Define to empty if `const' does not conform to ANSI C. */
#/* #undef const */

#
# Check for type sizes.
#
include(CheckTypeSize)
CHECK_TYPE_SIZE("short" SIZEOF_SHORT)
CHECK_TYPE_SIZE("int"   SIZEOF_INT)
CHECK_TYPE_SIZE("long"  SIZEOF_LONG)
CHECK_TYPE_SIZE("double"  SIZEOF_DOUBLE)
CHECK_TYPE_SIZE("long double"  SIZEOF_LONG_DOUBLE)
#message(STATUS "short  ${SIZEOF_SHORT}" )
#message(STATUS "int   ${SIZEOF_INT}" )
#message(STATUS "long  ${SIZEOF_LONG}" )
#message(STATUS "double ${SIZEOF_DOUBLE}" )
#message(STATUS "long double ${SIZEOF_LONG_DOUBLE}" )

#
# Check C-type for 32-bit integers.
#
if( ${SIZEOF_SHORT} EQUAL 4)
  set( nr_int32_t "short")
elseif( ${SIZEOF_INT} EQUAL 4)
  set( nr_int32_t "int")
elseif( ${SIZEOF_long} EQUAL 4)
  set( nr_int32_t "long")
endif()

#
# Check C-type for 16-bit integers.
#
if( ${SIZEOF_SHORT} EQUAL 2)
  set( nr_int16_t "short")
elseif( ${SIZEOF_INT} EQUAL 2)
  set( nr_int16_t "int")
elseif( ${SIZEOF_long} EQUAL 2)
  set( nr_int16_t "long")
endif()

#
# Check for double type.
#  * valid types are: double, float and long double.
#  * defines: nr_double_t,  The global type of double representation.
#  * defines: NR_DOUBLE_SIZE,  The size of the double representation.
#  * Use -DENABLE-DOUBLE="[float,double, long double]"
If( ENABLE-DOUBLE )
  # User defined
  set(DoubleType ${ENABLE-DOUBLE})

  # valid types
  set(ValidTypes "float" "double" "long double" )

  list(FIND ValidTypes ${DoubleType} HasType)
  IF ( HasType EQUAL -1)
    MESSAGE(FATAL_ERROR "Valid types are: ${ValidTypes}")
  ENDIF()

  # The global type of double representation.
  set( nr_double_t  DoubleType )
  CHECK_TYPE_SIZE(${DoubleType}  DoubleSize)

  # The size of the double representation.
  set( NR_DOUBLE_SIZE ${DoubleSize} )
else()
  # Default
  set(DoubleType "double")
  # The global type of double representation.
  set( nr_double_t ${DoubleType} )
  CHECK_TYPE_SIZE(${DoubleType}  DoubleSize)

  # The size of the double representation.
  set( NR_DOUBLE_SIZE ${DoubleSize} )
endif()
message(STATUS "using double type: ${DoubleType}; size: ${DoubleSize}")



#dnl Checks for library functions.
# Checks for library functions.
# TODO, do they need to be defined on the header?
# the definitions do not seem to be used anywhere.

#these seem to be handled in math/real.cpp
#ifndef HAVE_ROUND
#ifndef HAVE_TRUNC
#ifndef HAVE_ACOSH
#ifndef HAVE_ASINH

include(CheckFunctionExists)
set(REQUIRED_FUNCTIONS
  floor pow exp sqrt log10 log
  cos sin acos asin tan atan sinh
	cosh tanh fabs modf atan2 jn yn
  erf erfc round trunc acosh
  asinh strdup strerror strchr )
#check_function_exists ("${REQUIRED_FUNCTIONS}" TRUE)
foreach(func ${REQUIRED_FUNCTIONS})
  string(TOUPPER ${func} FNAME)
  CHECK_FUNCTION_EXISTS(${func} HAVE_${FNAME})
  #message(STATUS "${func}  --> ${HAVE_${FNAME}}")
endforeach()


#
# Checks for complex classes and functions.
#
# AC_CXX_NAMESPACES !!custom m4
# AC_CXX_HAVE_COMPLEX !!custom m4
# AC_CXX_HAVE_TR1_COMPLEX !!custom m4
# AC_CHECK_CXX_COMPLEX_FUNCS([cos cosh exp log log10 sin sinh sqrt tan tanh]) !!custom m4
# AC_CHECK_CXX_COMPLEX_FUNCS([acos acosh asin asinh atan atanh])
# AC_CHECK_CXX_COMPLEX_FUNCS([log2 norm])
# AC_CHECK_CXX_COMPLEX_POW
# AC_CHECK_CXX_COMPLEX_ATAN2         !failed, need libstdc?
# AC_CHECK_CXX_COMPLEX_FMOD          !failed:
# AC_CHECK_CXX_COMPLEX_POLAR
# AC_CHECK_CXX_COMPLEX_POLAR_COMPLEX !failed


#
# Namespace
#
# Check whether the compiler implements namespaces
#
TRY_COMPILE( HAVE_NAMESPACES
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/namespaces.cpp
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_NAMESPACES)
  message(SEND_ERROR "${PROJECT_NAME} requires an c++ compiler with namespace HAVE_NAMESPACES failed")# ${TRY_OUT}")
endif()


#
# Check whether the compiler has complex<T>
#
TRY_COMPILE( HAVE_COMPLEX
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_COMPLEX)
  message(SEND_ERROR "HAVE_COMPLEX failed")# ${TRY_OUT}")
endif()

#
# Check whether the compiler has TR1 compliant complex<T>
#
TRY_COMPILE( HAVE_TR1_COMPLEX
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/tr1_complex.cpp
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_TR1_COMPLEX)
  message(SEND_ERROR "HAVE_COMPLEX failed")# ${TRY_OUT}")
endif()

#
# Check for list of complex functions.
#
set(COMPLEX_FUNCS cos cosh exp log log10 sin sinh sqrt tan tanh
            acos acosh asin asinh atan atanh log2 norm)

# TODO any better way to scape the ";"
foreach(func ${COMPLEX_FUNCS})
  set(code
    "#ifdef HAVE_TR1_COMPLEX
    #include <tr1/complex>
      using namespace std\;
      using namespace std::tr1\;
    #else
    #include <complex>
    #ifdef HAVE_NAMESPACES
      using namespace std\;
    #endif
    #endif
    #ifdef log2
    #undef log2
    #endif

    int main() {
      complex<double> a\;
      ${func}(a)\;
      return 0\;
    }")

  FILE(WRITE ${CMAKE_BINARY_DIR}/cmake/test_${func}.cpp ${code})

  string(TOUPPER ${func} FNAME)

  message(STATUS "Checking HAVE_CXX_COMPLEX_${FNAME}")

  TRY_COMPILE( HAVE_CXX_COMPLEX_${FNAME}
          ${CMAKE_BINARY_DIR}
          ${CMAKE_BINARY_DIR}/cmake/test_${func}.cpp
          COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
          OUTPUT_VARIABLE TRY_OUT)
  if(NOT HAVE_CXX_COMPLEX_${FNAME})
    message(STATUS "HAVE_CXX_COMPLEX_${FNAME} failed")# ${TRY_OUT}")
  endif()
endforeach()


#
# Check whether you have the complex pow function.
#
TRY_COMPILE( HAVE_CXX_COMPLEX_POW
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex_pow.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_CXX_COMPLEX_POW)
  message(SEND_ERROR "HAVE_CXX_COMPLEX_POW failed")# ${TRY_OUT}")
endif()

#
# Check whether you have the complex atan2 function.
#
TRY_COMPILE( HAVE_CXX_COMPLEX_ATAN2
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex_atan2.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_CXX_COMPLEX_ATAN2)
  message(STATUS "HAVE_CXX_COMPLEX_ATAN2 failed ") #${TRY_OUT}")
endif()

#
# Check wheter you have the complex fmod function.
#
TRY_COMPILE( HAVE_CXX_COMPLEX_FMOD
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex_fmod.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_CXX_COMPLEX_FMOD)
  message(STATUS "HAVE_CXX_COMPLEX_FMOD failed ") #${TRY_OUT}")
endif()

#
# Check whether you have the complex polar (double, double) function.
#
TRY_COMPILE( HAVE_CXX_COMPLEX_POLAR
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex_polar.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_CXX_COMPLEX_POLAR)
  message(STATUS "HAVE_CXX_COMPLEX_POLAR failed ") #${TRY_OUT}")
endif()


#
# Check wheter you have the complex polar (complex, complex) function.
#
TRY_COMPILE( HAVE_CXX_COMPLEX_POLAR_COMPLEX
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex_polar_complex.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_CXX_COMPLEX_POLAR_COMPLEX)
  message(STATUS "HAVE_CXX_COMPLEX_POLAR_COMPLEX failed ") #${TRY_OUT}")
endif()


LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/..
                 ${CMAKE_BINARY_DIR}/src/math/ )

#set( LINKER_FLAGS " -lstdc++ " )
#set( CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS}")


find_package(BISON)
find_package(FLEX)

set(ParserTypes
  netlist
  dataset
  citi
	csv
  mdl
  touchstone
  zvr)

# Flex/Bison generated code:
#   parse_dataset.cpp scan_dataset.cpp
#   parse_citi.cpp scan_citi.cpp
#	  parse_csv.cpp scan_csv.cpp
#   parse_mdl.cpp scan_mdl.cpp
#   parse_touchstone.cpp scan_touchstone.cpp
#   parse_zvr.cpp scan_zvr.cpp

# TODO Double check the location where the generated scanner/parser should go
foreach(type ${ParserTypes})

  flex_target(scan${type} scan_${type}.l ${PROJECT_SOURCE_DIR}/scan_${type}.cpp)

  bison_target(parse${type}
               parse_${type}.y ${PROJECT_SOURCE_DIR}/parse_${type}.cpp
               COMPILE_FLAGS --defines="${PROJECT_SOURCE_DIR}/tokens_${type}.h" )

  add_flex_bison_dependency(scan${type} parse${type})

  set(flexInputs   ${flexInputs}   ${FLEX_scan${type}_INPUT}   )
  set(flexOutputs  ${flexOutputs}  ${FLEX_scan${type}_OUTPUTS} )
  set(bisonInputs  ${bisonInputs}  ${BISON_parse${type}_INPUT} )
  set(bisonOutputs ${bisonOutputs} ${BISON_parse${type}_OUTPUTS})

endforeach()

set(generated_SRC ${flexOutputs} ${bisonOutputs})

#message(STATUS "----> Scanner / Parser     ${generated_SRC}\n")
# message(STATUS "Qucsator -- Flex Inputs : ${flexInputs}")
# message(STATUS "Qucsator -- Flex Outputs: ${flexOutputs}")
# message(STATUS "Qucsator -- Bison Inputs : ${bisonInputs}")
# message(STATUS "Qucsator -- Bison Outputs: ${bisonOutputs}")


#
# Source code
#
SET(LIBQUCSATOR_SRC
  dataset.cpp
  vector.cpp
  object.cpp
	property.cpp
  variable.cpp
	strlist.cpp
  logging.c
  exception.cpp
  exceptionstack.cpp
	matvec.cpp
  matrix.cpp
  sweep.cpp
  check_dataset.cpp
  check_citi.cpp
  check_csv.cpp
  check_mdl.cpp
  check_touchstone.cpp
	check_zvr.cpp
)


#
# Configure the header config.h
#
CONFIGURE_FILE (
    "${CMAKE_SOURCE_DIR}/config.h.cmake"
    "${CMAKE_BINARY_DIR}/config.h"
)

#
# Build libquacsator
#
ADD_LIBRARY(qucsator ${LIBQUCSATOR_SRC} ${generated_SRC} )


ADD_SUBDIRECTORY( math )
ADD_SUBDIRECTORY( components )
ADD_SUBDIRECTORY( converter )
ADD_SUBDIRECTORY( interface )


SET(QUCSATOR_SRC
  ucs.cpp circuit.cpp check_netlist.cpp operatingpoint.cpp
	parse_netlist.cpp scan_netlist.cpp node.cpp net.cpp input.cpp
	analysis.cpp spsolver.cpp dcsolver.cpp nodelist.cpp environment.cpp
	parasweep.cpp
  equation.cpp  # <= depends on gperfapphash.cpp
  evaluate.cpp acsolver.cpp
	trsolver.cpp transient.cpp integrator.cpp nodeset.cpp hbsolver.cpp
	spline.cpp fourier.cpp pair.cpp characteristic.cpp history.cpp
	range.cpp devstates.cpp differentiate.cpp module.cpp receiver.cpp
	interpolator.cpp )


#
# Template classes
#
SET(TEMPLATES tmatrix.h eqnsys.h nasolver.h states.h tvector.h
	            ptrlist.h tridiag.h hash.h valuelist.h nasolution.h )


#
# Check for sed
#
find_program (SED_TOOL NAMES sed)
if (NOT SED_TOOL)
	message (FATAL_ERROR "Unable to find sed")
endif (NOT SED_TOOL)

#
# Check for gperf
#
find_program (GPERF_TOOL NAMES gperf)
if (NOT GPERF_TOOL)
	message (FATAL_ERROR "Unable to find gperf")
endif (NOT GPERF_TOOL)

#
# Replace 'evaluate::[whatever]' by NULL
#
#	* evaluate.h (class evaluate): New class implementing the
#	actual evaluation function (applications) for the equations
#	in Qucs.
#
add_custom_command(
  OUTPUT gperfappgen.h
  COMMAND ${SED_TOOL} -e 's/evaluate::[a-zA-Z0-9_]*/NULL/g'
    < ${CMAKE_CURRENT_SOURCE_DIR}/applications.h
    > ${CMAKE_CURRENT_BINARY_DIR}/gperfappgen.h
  DEPENDS ${applications.h}
)

#
# Compile gperfappgen
# * used to generate gperf input file (used in qucsator)
#
set(gperf_SRC gperfappgen.cpp  gperfappgen.h )

add_executable(gperfappgen ${gperf_SRC})

#
# Run gperfappgen, pipe to gperf input to gperfapphash.gph
#
add_custom_command(
  OUTPUT gperfapphash.gph
  COMMAND gperfappgen > ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.gph
  DEPENDS ${gperfappgen}
)

#
# Run gperf, create hash table.
#  * -I, Include the necessary system include files at the beginning of the code.
#  * -m, Perform multiple iterations to minimize generated table.
#  * Replace '{""}' by '{"",0}; (why?)
#
ADD_CUSTOM_COMMAND(
    OUTPUT gperfapphash.cpp
    COMMAND ${GPERF_TOOL} -I -m 8 ${CMAKE_CURRENT_SOURCE_DIR}/gperfapphash.gph > temp.gperf
    COMMAND ${SED_TOOL} -e 's/{""},/{"",0},/g' < temp.gperf > ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.cpp
    DEPENDS gperfapphash.gph
)

#  target <- source (includea)
# equation.cpp: gperfapphash.cpp
#
# noinst_PROGRAMS = gperfappgen
# gperfappgen_SOURCES = gperfappgen.cpp

# for cleaning (autogenerated)
#set(gperf_FILES gperfapphash.cpp gperfapphash.gph gperfappgen.h)

# Force gperf to run before include into equation.cpp
ADD_CUSTOM_TARGET( equation DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.cpp equation.cpp )

# target library and binary names must be different, use target property to rename
ADD_EXECUTABLE(qucsator-bin ${QUCSATOR_SRC} ${TEMPLATES} )

SET_TARGET_PROPERTIES(qucsator-bin PROPERTIES OUTPUT_NAME qucsator)

ADD_DEPENDENCIES(qucsator-bin equation)

TARGET_LINK_LIBRARIES( qucsator-bin qucsator qucsmath component microstrip devices digital verilog interface )

#
# Handle install
#
INSTALL(TARGETS qucsator-bin DESTINATION bin)

