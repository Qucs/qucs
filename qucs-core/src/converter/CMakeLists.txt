
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

# generated files are named like:
#   - parse_spice.cpp scan_spice.cpp tokens_spice.h

# clear list of generated files
SET(conv_generated)
FOREACH( type ${ParserTypes} )

  # Create custom Bison
  SET(BO_${type}
    ${CMAKE_CURRENT_BINARY_DIR}/parse_${type}.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/tokens_${type}.h )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${BO_${type}}
    COMMAND ${BISON_EXECUTABLE}
              --defines=${CMAKE_CURRENT_BINARY_DIR}/parse_${type}.hpp
              --output=${CMAKE_CURRENT_BINARY_DIR/}parse_${type}.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/parse_${type}.ypp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parse_${type}.ypp
    )

  # Create custom Flex
  SET(FI_${type} ${CMAKE_CURRENT_SOURCE_DIR}/scan_${type}.lpp )
  SET(FO_${type} ${CMAKE_CURRENT_BINARY_DIR}/scan_${type}.cpp )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${FO_${type}}
    COMMAND ${FLEX_EXECUTABLE}
            --outfile=${FO_${type}}
            ${FI_${type}}
    DEPENDS ${BO_${type}} ${FI_${type}}
    )
  SET(conv_generated ${conv_generated} ${FO_${type}})
  SET(conv_generated ${conv_generated} ${BO_${type}})
ENDFOREACH()
FOREACH( gfile ${conv_generated} )
  SET_SOURCE_FILES_PROPERTIES(${gfile} PROPERTIES GENERATED TRUE)
ENDFOREACH()


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
