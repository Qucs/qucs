PROJECT(qucsator CXX C)

#AC_INIT(qucs-core,0.0.18,qucs-bugs@lists.sourceforge.net)
#AC_CONFIG_SRCDIR([src/circuit.cpp])
#AC_CONFIG_HEADERS([config.h])


## need to generate config


# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/math  #precision.h
                     .. #config.h
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
# Lifted the cmake checks from gd-libdg repo
# https://bitbucket.org/libgd/gd-libgd
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
  message(STATUS "${header}  --> ${HAVE_${base}_H}")
endforeach()


# Checks for typedefs, structures, and compiler characteristics.
#AC_C_CONST  !!obsolete
#AC_C_CONST "This macro is
#obsolescent, as current C compilers support `const'.  New programs
#need not use this macro."
#/* Define to empty if `const' does not conform to ANSI C. */
#/* #undef const */


include(CheckTypeSize)
#set(TYPES_CHECKED int8_t uint8_t int16_t uint16_t int32_t uint32_t int64_t
#    uint64_t uid_t mode_t off_t pid_t size_t)
#set(CMAKE_EXTRA_INCLUDE_FILES stdint.h)
#foreach(TYPE ${TYPES_CHECKED})
#  string(TOUPPER ${TYPE} TYPE_SIZE_VAR)
#  CHECK_TYPE_SIZE(${TYPE} ${TYPE_SIZE_VAR})
#  message(STATUS "${TYPE}    ${TYPE_SIZE_VAR}" )
#endforeach()


# Check for type sizes.
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


#AC_DEFINE_UNQUOTED(nr_int32_t, $type32, [C-type for 32-bit integers.])
if( ${SIZEOF_SHORT} EQUAL 4)
  set( nr_int32_t "short")
elseif( ${SIZEOF_INT} EQUAL 4)
  set( nr_int32_t "int")
elseif( ${SIZEOF_long} EQUAL 4)
  set( nr_int32_t "long")
endif()

#AC_DEFINE_UNQUOTED(nr_int16_t, $type16, [C-type for 16-bit integers.])
if( ${SIZEOF_SHORT} EQUAL 2)
  set( nr_int16_t "short")
elseif( ${SIZEOF_INT} EQUAL 2)
  set( nr_int16_t "int")
elseif( ${SIZEOF_long} EQUAL 2)
  set( nr_int16_t "long")
endif()


#dnl Check for double type.
# valid types are: double, float and long double.
# defines: nr_double_t,  The global type of double representation.
# defines: NR_DOUBLE_SIZE,  The size of the double representation.

# Use -DENABLE-DOUBLE=[float,double, long double]
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
  floor pow exp sqrt log10 log cos sin acos asin tan atan sinh
	cosh tanh fabs modf atan2 jn yn erf erfc round trunc acosh
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

 #AC_CXX_NAMESPACES !!custom m4
 #AC_CXX_HAVE_COMPLEX !!custom m4
 #AC_CXX_HAVE_TR1_COMPLEX !!custom m4
 #AC_CHECK_CXX_COMPLEX_FUNCS([cos cosh exp log log10 sin sinh sqrt tan tanh]) !!custom m4
 #AC_CHECK_CXX_COMPLEX_FUNCS([acos acosh asin asinh atan atanh])
 #AC_CHECK_CXX_COMPLEX_FUNCS([log2 norm])
 #AC_CHECK_CXX_COMPLEX_POW
 #AC_CHECK_CXX_COMPLEX_ATAN2         !failed, need libstdc?
 #AC_CHECK_CXX_COMPLEX_FMOD          !failed:
 #AC_CHECK_CXX_COMPLEX_POLAR
 #AC_CHECK_CXX_COMPLEX_POLAR_COMPLEX !failed


#
# Namespace
#
#AC_DEFUN([AC_CXX_NAMESPACES],
#[AC_CACHE_CHECK(whether the compiler implements namespaces,
#  AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
#fi
TRY_COMPILE( HAVE_NAMESPACES
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/namespaces.cpp
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_NAMESPACES)
  message(SEND_ERROR "HAVE_NAMESPACES failed ${TRY_OUT}")
  message(SEND_ERROR "requires an c++ compiler with namespace HAVE_NAMESPACES failed ${TRY_OUT}")
#  AC_MSG_ERROR([The $PACKAGE package requires an c++ compiler with namespace.])
endif()



#[AC_CACHE_CHECK(whether the compiler has complex<T>,
#  AC_DEFINE(HAVE_COMPLEX,[1],[Define if the compiler has complex<T>.])
TRY_COMPILE( HAVE_COMPLEX
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_COMPLEX)
  message(SEND_ERROR "HAVE_COMPLEX failed ${TRY_OUT}")
endif()


#[AC_CACHE_CHECK(whether the compiler has TR1 compliant complex<T>,
#	    [Define if the compiler has TR1 compliant complex<T>.])
TRY_COMPILE( HAVE_TR1_COMPLEX
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/tr1_complex.cpp
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_TR1_COMPLEX)
  message(SEND_ERROR "HAVE_COMPLEX failed ${TRY_OUT}")
endif()


# [Define to 1 if you have the complex `]m4_defn([AC_Func])[' function.])])])

#AC_CHECK_CXX_COMPLEX_FUNCS([log2 norm])
set(COMPLEX_FUNCS cos cosh exp log log10 sin sinh sqrt tan tanh
            acos acosh asin asinh atan atanh log2 norm)

# TODO any better way to scape the ";"
foreach(fu ${COMPLEX_FUNCS})
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
      ${fu}(a)\;
      return 0\;
    }")

  FILE(WRITE ${CMAKE_SOURCE_DIR}/cmake/test_${fu}.cpp ${code})

  string(TOUPPER ${fu} FNAME)

  message(STATUS "Checking HAVE_CXX_COMPLEX_${FNAME}")

  TRY_COMPILE( HAVE_CXX_COMPLEX_${FNAME}
          ${CMAKE_BINARY_DIR}
          ${CMAKE_SOURCE_DIR}/cmake/test_${fu}.cpp
          COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
          OUTPUT_VARIABLE TRY_OUT)
  if(NOT HAVE_CXX_COMPLEX_${FNAME})
    message(STATUS "HAVE_CXX_COMPLEX_${FNAME} failed")# ${TRY_OUT}")
  endif()
endforeach()


#     [AC_DEFINE(HAVE_CXX_COMPLEX_POW, 1,
#                [Define to 1 if you have the complex pow function.])],
TRY_COMPILE( HAVE_CXX_COMPLEX_POW
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex_pow.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_CXX_COMPLEX_POW)
  message(SEND_ERROR "HAVE_CXX_COMPLEX_POW failed")# ${TRY_OUT}")
endif()

#     [AC_DEFINE(HAVE_CXX_COMPLEX_ATAN2, 1,
#                [Define to 1 if you have the complex atan2 function.])],
#
# OK I don't have it, should it error out? or just set the defines?
TRY_COMPILE( HAVE_CXX_COMPLEX_ATAN2
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex_atan2.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_CXX_COMPLEX_ATAN2)
  message(STATUS "HAVE_CXX_COMPLEX_ATAN2 failed ") #${TRY_OUT}")
endif()

#     [AC_DEFINE(HAVE_CXX_COMPLEX_FMOD, 1,
#                [Define to 1 if you have the complex fmod function.])],
TRY_COMPILE( HAVE_CXX_COMPLEX_FMOD
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex_fmod.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_CXX_COMPLEX_FMOD)
  message(STATUS "HAVE_CXX_COMPLEX_FMOD failed ") #${TRY_OUT}")
endif()

#     [AC_DEFINE(HAVE_CXX_COMPLEX_POLAR, 1,
#                [Define to 1 if you have the complex polar (double, double) function.])],
TRY_COMPILE( HAVE_CXX_COMPLEX_POLAR
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex_polar.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_CXX_COMPLEX_POLAR)
  message(STATUS "HAVE_CXX_COMPLEX_POLAR failed ") #${TRY_OUT}")
endif()


#     [AC_DEFINE(HAVE_CXX_COMPLEX_POLAR_COMPLEX, 1,
#                [Define to 1 if you have the complex polar (complex, complex) function.])],
TRY_COMPILE( HAVE_CXX_COMPLEX_POLAR_COMPLEX
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/cmake/complex_polar_complex.cpp
        COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
        OUTPUT_VARIABLE TRY_OUT)
if(NOT HAVE_CXX_COMPLEX_POLAR_COMPLEX)
  message(STATUS "HAVE_CXX_COMPLEX_POLAR_COMPLEX failed ") #${TRY_OUT}")
endif()


## try to compile a file that should compile
#try_compile(SHOULD_PASS
#    ${CMAKE_BINARY_DIR}
#    ${CMAKE_SOURCE_DIR}/cmake/pass.c
#    OUTPUT_VARIABLE TRY_OUT)
#if(NOT SHOULD_PASS)
#  message(SEND_ERROR "should pass failed ${TRY_OUT}")
#endif()


SET(QUCSATOR_SRC
  dataset.cpp
  vector.cpp
  object.cpp
	property.cpp
  variable.cpp
	strlist.cpp
  logging.c
  exception.cpp exceptionstack.cpp
	matvec.cpp matrix.cpp
  sweep.cpp

  check_dataset.cpp
  check_citi.cpp
  check_csv.cpp
  check_mdl.cpp
  check_touchstone.cpp
	check_zvr.cpp

  parse_dataset.cpp scan_dataset.cpp
  parse_citi.cpp scan_citi.cpp
	parse_csv.cpp scan_csv.cpp
  parse_mdl.cpp scan_mdl.cpp
  parse_touchstone.cpp scan_touchstone.cpp
  parse_zvr.cpp scan_zvr.cpp )
# ok

ADD_SUBDIRECTORY( components )
ADD_SUBDIRECTORY( converter )
ADD_SUBDIRECTORY( math )
#ADD_SUBDIRECTORY( interface )



# works

#INCLUDES = -I$(top_srcdir)/src/math -I$(top_srcdir)/src/components -I$(top_srcdir)/src/interface
#qucsator_LDADD = libqucsator.a		      \
#	math/libqucsmath.a	              \
#	components/libcomponent.a	      \
#	components/microstrip/libmicrostrip.a \
#	components/devices/libdevices.a	      \
#	components/digital/libdigital.a       \
#	components/verilog/libverilog.a \
#	interface/libinterface.a
#

#ADD_EXECUTABLE(qucsator ${QUCSATOR_SRC})

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/..
                 ${CMAKE_BINARY_DIR}/src/math/ )

#set( LINKER_FLAGS " -lstdc++ " )
#set( CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS}")

#qucsconv_LDADD = ../libqucsator.a ../math/libqucsmath.a
#TARGET_LINK_LIBRARIES(qucsator qucsmath microstrip devices digital verilog)



# TODO, don' know how to handle this
#/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
#   `char[]'. */
#/* #undef YYTEXT_POINTER */
# potential workaround at:
# http://brlcad.org/websvn/filedetails.php?repname=BRL-CAD&path=%2Fbrlcad%2Ftrunk%2Fsrc%2Fother%2Fstep%2FCMake%2FFindLEX.cmake&rev=49763&peg=49763




find_package(BISON)
find_package(FLEX)

bison_target(NETLISTParser parse_netlist.y ${CMAKE_CURRENT_BINARY_DIR}/parse_netlist.cpp
             HEADER ${CMAKE_CURRENT_BINARY_DIR}/tokens_netlist.h )
             #VERBOSE ${CMAKE_CURRENT_BINARY_DIR}/cmd_parser.output
             #COMPILE_FLAGS "--debug")
flex_target(NETLISTScanner scan_netlist.l ${CMAKE_CURRENT_BINARY_DIR}/scan_netlist.cc)
add_flex_bison_dependency(NETLISTScanner NETLISTParser)


#BISON_TARGET(parsenetlist parse_netlist.y ${CMAKE_CURRENT_BINARY_DIR}/parser_netlist.cpp
#             COMPILE_FLAGS
#             --defines="${PROJECT_BINARY_DIR}/tokens_netlist.h")
#FLEX_TARGET(scannetlist scan_netlist.l  ${CMAKE_CURRENT_BIANRY_DIR}/scan_netlist.cpp)

#SET(GENERATED_FLEX_BISON
set(generated_SRCS
    ${BISON_NETLISTParser_INPUT}
    ${FLEX_NETLISTScanner_INPUT}
    ${BISON_NETLISTParser_OUTPUTS}
    ${FLEX_NETLISTScanner_OUTPUTS} )
  #scan_netlist.cpp parse_netlist.cpp tokens_netlist.h )
#  scan_dataset.cpp parse_dataset.cpp tokens_dataset.h
#	scan_touchstone.cpp parse_touchstone.cpp tokens_touchstone.h
#  scan_citi.cpp	parse_citi.cpp tokens_citi.h  ) #parse_zvr.cpp
#	scan_zvr.cpp tokens_zvr.h parse_mdl.cpp	scan_mdl.cpp tokens_mdl.h \
#	parse_csv.cpp tokens_csv.h scan_csv.cpp qucsdefs.h

#scan_netlist.cpp: scan_netlist.l
#	$(LEX) -o$@ $<
#parse_netlist.cpp: parse_netlist.y
#	$(YACC) -v -o $@ --defines=tokens_netlist.h $<





# configure the header config.h
CONFIGURE_FILE (
    "${CMAKE_SOURCE_DIR}/config.h.cmake"
    "${CMAKE_BINARY_DIR}/config.h"
)




ADD_LIBRARY(qucsator ${QUCSATOR_SRC} ${generated_SRCS})
