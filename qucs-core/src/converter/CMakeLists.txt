
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/math
                     ${CMAKE_CURRENT_BINARY_DIR} ) # qucdefs.h


SET(QUCSCONV_SRC
	check_spice.cpp
	check_vcd.cpp
	matlab_producer.cpp
  csv_producer.cpp
  qucs_producer.cpp
  qucsconv.cpp
  touchstone_producer.cpp
)

SET(ParserTypes
  spice
  vcd
)

# clear list of generated files
SET(conv_generated)

# generated files are named like:
#   - parse_spice.cpp scan_spice.cpp tokens_spice.h

# TODO Double check the location where the generated scanner/parser should go
FOREACH(type ${ParserTypes})

  FLEX_TARGET(scan${type} scan_${type}.l ${PROJECT_SOURCE_DIR}/scan_${type}.cpp)

  BISON_TARGET(parse${type}
               parse_${type}.y ${PROJECT_SOURCE_DIR}/parse_${type}.cpp
               COMPILE_FLAGS --defines="${PROJECT_SOURCE_DIR}/tokens_${type}.h" )

  ADD_FLEX_BISON_DEPENDENCY(scan${type} parse${type})

  SET(convflexInputs   ${convflexInputs}   ${FLEX_scan${type}_INPUT}   )
  SET(convflexOutputs  ${convflexOutputs}  ${FLEX_scan${type}_OUTPUTS} )
  SET(convbisonInputs  ${convbisonInputs}  ${BISON_parse${type}_INPUT} )
  SET(convbisonOutputs ${convbisonOutputs} ${BISON_parse${type}_OUTPUTS})

ENDFOREACH()

SET(conv_generated ${convflexOutputs} ${convbisonOutputs})

#message(STATUS "----> Scanner / Parser ${conv_generated}\n")
#message(STATUS "Qucsconv -- Flex Inputs  : ${convflexInputs}")
#message(STATUS "Qucsconv -- Flex Outputs : ${convflexOutputs}")
#message(STATUS "Qucsconv -- Bison Inputs : ${convbisonInputs}")
#message(STATUS "Qucsconv -- Bison Outputs: ${convbisonOutputs}")

#
# qucsator is needed before qucsconv can be compiled
# run qucsator -l > pipe to qucsdef.h
#
ADD_CUSTOM_COMMAND(
  OUTPUT qucsdefs.h
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../qucsator -l > ${CMAKE_CURRENT_BINARY_DIR}/qucsdefs.h
  DEPENDS qucsator
)

ADD_CUSTOM_TARGET( defs DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/qucsdefs.h)

ADD_EXECUTABLE(qucsconv ${QUCSCONV_SRC} ${conv_generated} )

ADD_DEPENDENCIES(qucsconv defs)

TARGET_LINK_LIBRARIES(qucsconv libqucs)

#
# Handle installation
#
INSTALL(TARGETS qucsconv DESTINATION bin)
