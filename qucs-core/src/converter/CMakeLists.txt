

cmake_minimum_required(VERSION 2.8)
project (qucsconv)

find_package(BISON)
find_package(FLEX)

# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/../math  #precision.h
                     ../.. # qucs-core
                     ..)   # qucs-core/src



LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/..
                 ${CMAKE_BINARY_DIR}/src/math/ )


SET(QUCSCONV_SRC qucsconv.cpp parse_spice.cpp scan_spice.cpp
	check_spice.cpp qucs_producer.cpp parse_vcd.cpp scan_vcd.cpp
	check_vcd.cpp csv_producer.cpp touchstone_producer.cpp
	matlab_producer.cpp)


#noinst_HEADERS = tokens_spice.h check_spice.h qucs_producer.h check_vcd.h \
#	tokens_vcd.h csv_producer.h touchstone_producer.h matlab_producer.h

#generated_FILES = parse_spice.cpp scan_spice.cpp tokens_spice.h \
#	parse_vcd.cpp scan_vcd.cpp tokens_vcd.h

#scan_spice.cpp: scan_spice.l
#	$(LEX) -o$@ $<

#parse_spice.cpp: parse_spice.y
#	$(YACC) -v -o $@ --defines=tokens_spice.h $<

BISON_TARGET(parsespice parse_spice.y ${CMAKE_CURRENT_BINARY_DIR}/parser_spice.cpp)
FLEX_TARGET(scanspice scan_spice.l  ${CMAKE_CURRENT_BIANRY_DIR}/scan_spice.cpp)
#ADD_FLEX_BISON_DEPENDENCY(parsespice scanspice)


#scan_vcd.cpp: scan_vcd.l
#	$(LEX) -o$@ $<

#parse_vcd.cpp: parse_vcd.y
#	$(YACC) -v -o $@ --defines=tokens_vcd.h $<

BISON_TARGET(parsevcd parse_vcd.y ${CMAKE_CURRENT_BINARY_DIR}/parser_vcd.cpp)
FLEX_TARGET(scanvcd scan_vcd.l  ${CMAKE_CURRENT_BIANRY_DIR}/scan_vcd.cpp)


ADD_EXECUTABLE(qucsconv ${QUCSCONV_SRC})


#qucsconv_LDADD = ../libqucsator.a ../math/libqucsmath.a
TARGET_LINK_LIBRARIES(qucsconv qucsator qucsmath)

#${FLEX_parsespice_LIBRARIES}
#${FLEX_parsesvdc_LIBRARIES})

#BISON_TARGET(MyParser yacc.y ${CMAKE_CURRENT_BINARY_DIR}/y.tab.c)
#FLEX_TARGET(MyScanner lex.l  ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
#ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)
#
#include_directories(${CMAKE_CURRENT_BINARY_DIR})
#add_executable(LexYacc
#  ${BISON_MyParser_OUTPUTS}
#  ${FLEX_MyScanner_OUTPUTS})
#target_link_libraries(LexYacc ${FLEX_LIBRARIES} ${BISON_LIBRARIES})

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
