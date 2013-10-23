

cmake_minimum_required(VERSION 2.8)
project (qucsconv)


# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/../math  #precision.h
                     ../.. # qucs-core
                     ..)   # qucs-core/src



LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/..
                 ${CMAKE_BINARY_DIR}/src/math/ )


SET(QUCSCONV_SRC
  qucsconv.cpp
	check_spice.cpp
	check_vcd.cpp
  qucs_producer.cpp
  csv_producer.cpp
  touchstone_producer.cpp
	matlab_producer.cpp)

# generated
  #parse_spice.cpp scan_spice.cpp
  #parse_vcd.cpp scan_vcd.cpp

#noinst_HEADERS = tokens_spice.h check_spice.h qucs_producer.h check_vcd.h \
#	tokens_vcd.h csv_producer.h touchstone_producer.h matlab_producer.h

#generated_FILES = parse_spice.cpp scan_spice.cpp tokens_spice.h \
#	parse_vcd.cpp scan_vcd.cpp tokens_vcd.h


#BISON_TARGET(parsespice parse_spice.y
#    ${CMAKE_CURRENT_BINARY_DIR}/parser_spice.cpp
#    COMPILE_FLAGS --defines="${PROJECT_SOURCE_DIR}/tokens_spice.h" )
#
#FLEX_TARGET(scanspice scan_spice.l  ${CMAKE_CURRENT_BIANRY_DIR}/scan_spice.cpp)
#ADD_FLEX_BISON_DEPENDENCY(parsespice scanspice)
#
#set(bisonInputs  ${bisonInputs}  ${BISON_parsespice_INPUT} )
#set(bisonOutputs ${bisonOutputs} ${BISON_parsespice_OUPUTS})
#set(flexInputs   ${flexInputs}   ${FLEX_scanspice_INPUT}   )
#set(flexOutputs  ${flexOutputs}  ${FLEX_scanspice_OUTPUTS} )
#
#
#
#BISON_TARGET(parsevcd parse_vcd.y
#    ${CMAKE_CURRENT_BINARY_DIR}/parser_vcd.cpp
#    COMPILE_FLAGS --defines="${PROJECT_SOURCE_DIR}/tokens_vcd.h" )
#
#FLEX_TARGET(scanvcd scan_vcd.l  ${CMAKE_CURRENT_BIANRY_DIR}/scan_vcd.cpp)
#ADD_FLEX_BISON_DEPENDENCY(parsevcd scanvcd)


find_package(BISON)
find_package(FLEX)

set(ParserTypes
  spice
  vcd)

set(generated_SRC)

# TODO Double check the location where the generated scanner/parser should go
foreach(type ${ParserTypes})

  flex_target(scan${type} scan_${type}.l ${PROJECT_SOURCE_DIR}/scan_${type}.cpp)

  bison_target(parse${type}
               parse_${type}.y ${PROJECT_SOURCE_DIR}/parse_${type}.cpp
               COMPILE_FLAGS --defines="${PROJECT_SOURCE_DIR}/tokens_${type}.h" )

  add_flex_bison_dependency(scan${type} parse${type})

  set(convflexInputs   ${convflexInputs}   ${FLEX_scan${type}_INPUT}   )
  set(convflexOutputs  ${convflexOutputs}  ${FLEX_scan${type}_OUTPUTS} )
  set(convbisonInputs  ${convbisonInputs}  ${BISON_parse${type}_INPUT} )
  set(convbisonOutputs ${convbisonOutputs} ${BISON_parse${type}_OUTPUTS})

endforeach()

set(conv_generated ${convflexOupus} ${convbisonOutputs})

#message(STATUS "----> Scanner / Parser ${conv_generated}\n")
message(STATUS "Qucsconv -- Flex Inputs  : ${convflexInputs}")
message(STATUS "Qucsconv -- Flex Outputs : ${convflexOutputs}")
message(STATUS "Qucsconv -- Bison Inputs : ${convbisonInputs}")
message(STATUS "Qucsconv -- Bison Outputs: ${convbisonOutputs}")


#parse_spice.cpp scan_spice.cpp tokens_spice.h \
#	parse_vcd.cpp scan_vcd.cpp tokens_vcd.h


ADD_EXECUTABLE(qucsconv ${QUCSCONV_SRC} ${generated_FILES})


#qucsconv_LDADD = ../libqucsator.a ../math/libqucsmath.a
TARGET_LINK_LIBRARIES(qucsconv qucsator qucsmath)


#
## Create target for the parser
# ADD_CUSTOM_TARGET(FooParser echo "Creating parser.c")
#
## Create custom command for flex/lex (note the outputs)
# ADD_CUSTOM_COMMAND(
#   SOURCE ${Foo_SOURCE_DIR}/src/lexer.l
#   COMMAND ${FLEX_EXECUTABLE}
#   ARGS -o${Foo_BINARY_DIR}/src/lexer.c
#        ${Foo_SOURCE_DIR}/src/lexer.l
#   TARGET FooParser
#   OUTPUTS ${Foo_BINARY_DIR}/src/lexer.c)
#
## Create custom command for bison/yacc (note the DEPENDS)
# ADD_CUSTOM_COMMAND(
#   SOURCE ${Foo_SOURCE_DIR}/src/parser.y
#   COMMAND ${BISON_EXECUTABLE}
#   ARGS -y ${Foo_SOURCE_DIR}/src/parser.y
#        -o ${Foo_BINARY_DIR}/src/parser.c
#   TARGET FooParser
#   DEPENDS ${Foo_BINARY_DIR}/src/lexer.c
#   OUTPUTS ${Foo_BINARY_DIR}/src/parser.c)
#
## Add parser.c to the list of sources
# SET(Foo_SRCS ${Foo_SRCS} ${Foo_BINARY_DIR}/src/parser.c)
#
## Since parser.c does not exists yet when cmake is run, mark
## it as generated
# SET_SOURCE_FILES_PROPERTIES(${Foo_BINARY_DIR}/src/parser.c GENERATED)
#
## Include binary directory to include lexer.c in parser.c
# INCLUDE_DIRECTORIES(${Foo_BINARY_DIR}/src)
#
