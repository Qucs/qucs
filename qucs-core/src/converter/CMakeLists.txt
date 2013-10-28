

cmake_minimum_required(VERSION 2.8)
project (qucsconv)


# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/../math  #precision.h
                     ../.. # qucs-core
                     ..   # qucs-core/src
                     ${CMAKE_CURRENT_BINARY_DIR} ) # qucdefs.h



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

#set(hdrs check_spice.h check_vcd.h)

#noinst_HEADERS = tokens_spice.h qucs_producer.h  \
#	tokens_vcd.h csv_producer.h touchstone_producer.h matlab_producer.h


find_package(BISON)
find_package(FLEX)

set(ParserTypes
  spice
  vcd)

set(conv_generated)
# TODO Double check the location where the generated scanner/parser should go
#generated_FILES = parse_spice.cpp scan_spice.cpp tokens_spice.h \
#	parse_vcd.cpp scan_vcd.cpp tokens_vcd.h
  #parse_spice.cpp scan_spice.cpp
  #parse_vcd.cpp scan_vcd.cpp
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

set(conv_generated ${convflexOutputs} ${convbisonOutputs})

#message(STATUS "----> Scanner / Parser ${conv_generated}\n")
# message(STATUS "Qucsconv -- Flex Inputs  : ${convflexInputs}")
# message(STATUS "Qucsconv -- Flex Outputs : ${convflexOutputs}")
# message(STATUS "Qucsconv -- Bison Inputs : ${convbisonInputs}")
# message(STATUS "Qucsconv -- Bison Outputs: ${convbisonOutputs}")

#
# qucsator is needed before qucsconv can be compiled
# run qucsator -l > pipe to qucsdef.h
#
add_custom_command(
  OUTPUT qucsdefs.h
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../qucsator -l > ${CMAKE_CURRENT_BINARY_DIR}/qucsdefs.h
  DEPENDS qucsator-bin
)

ADD_CUSTOM_TARGET( defs DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/qucsdefs.h)

ADD_EXECUTABLE(qucsconv ${QUCSCONV_SRC} ${conv_generated} )

ADD_DEPENDENCIES(qucsconv defs)

# link libraries: libqucsator libqucsmath
TARGET_LINK_LIBRARIES(qucsconv qucsator qucsmath)

