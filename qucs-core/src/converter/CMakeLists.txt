include_directories(
  ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/math
  ${CMAKE_CURRENT_BINARY_DIR}) # qucdefs.h

set(QUCSCONV_SRC
    check_spice.cpp
    check_vcd.cpp
    matlab_producer.cpp
    csv_producer.cpp
    qucs_producer.cpp
    qucsconv.cpp
    touchstone_producer.cpp)

set(ParserTypes spice vcd)

# generated files are named like: - parse_spice.cpp scan_spice.cpp
# tokens_spice.h

# clear list of generated files
set(conv_generated)
foreach(type ${ParserTypes})

  # Create custom Bison
  set(BO_${type} ${CMAKE_CURRENT_BINARY_DIR}/parse_${type}.cpp
                 ${CMAKE_CURRENT_BINARY_DIR}/tokens_${type}.h)
  add_custom_command(
    OUTPUT ${BO_${type}}
    COMMAND
      ${BISON_EXECUTABLE}
      --defines=${CMAKE_CURRENT_BINARY_DIR}/parse_${type}.hpp
      --output=${CMAKE_CURRENT_BINARY_DIR/}parse_${type}.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/parse_${type}.ypp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parse_${type}.ypp)

  # Create custom Flex
  set(FI_${type} ${CMAKE_CURRENT_SOURCE_DIR}/scan_${type}.lpp)
  set(FO_${type} ${CMAKE_CURRENT_BINARY_DIR}/scan_${type}.cpp)
  add_custom_command(
    OUTPUT ${FO_${type}}
    COMMAND ${FLEX_EXECUTABLE} --outfile=${FO_${type}} ${FI_${type}}
    DEPENDS ${BO_${type}} ${FI_${type}})
  set(conv_generated ${conv_generated} ${FO_${type}})
  set(conv_generated ${conv_generated} ${BO_${type}})
endforeach()
foreach(gfile ${conv_generated})
  set_source_files_properties(${gfile} PROPERTIES GENERATED TRUE)
endforeach()

#
# qucsator is needed before qucsconv can be compiled run qucsator -l > pipe to
# qucsdef.h
#
add_custom_command(
  OUTPUT qucsdefs.h
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../qucsator -l >
          ${CMAKE_CURRENT_BINARY_DIR}/qucsdefs.h
  DEPENDS qucsator)

add_custom_target(defs DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/qucsdefs.h)

add_executable(qucsconv ${QUCSCONV_SRC} ${conv_generated})

add_dependencies(qucsconv defs)

target_link_libraries(qucsconv libqucsator ${CMAKE_DL_LIBS})

#
# Handle installation
#
install(TARGETS qucsconv DESTINATION bin)
