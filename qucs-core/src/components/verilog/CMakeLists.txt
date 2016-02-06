
INCLUDE_DIRECTORIES( ${qucs-core_SOURCE_DIR}
                     ${qucs-core_CURRENT_SOURCE_DIR}
                     ${qucs-core_SOURCE_DIR}/src/math
                     ${qucs-core_SOURCE_DIR}/src/components             # component.h
                     ${qucs-core_SOURCE_DIR}/src/components/devices )   # devices.h


# TODO, test if VA files change, should recompile

SET( VA_FILES
  andor4x2
  andor4x3
  andor4x4
  binarytogrey4bit
  comp_1bit
  comp_2bit
  comp_4bit
  dff_SR
  DLS_1ton
  DLS_nto1
  dmux2to4
  dmux3to8
  dmux4to16
  EKV26MOS
  fa1b
  fa2b
  gatedDlatch
  greytobinary4bit
  ha1b
  hpribin4bit
  jkff_SR
  log_amp
  logic_0
  logic_1
  MESFET
  mod_amp
  mux2to1
  mux4to1
  mux8to1
  nigbt
  pad2bit
  pad3bit
  pad4bit
  photodiode
  phototransistor
  potentiometer
  tff_SR
  vcresistor
)


# XML sripts need to build, the order matters
SET(XML_BUILD
  analogfunction.xml
  qucsVersion.xml
  qucsMODULEcore.xml
  qucsMODULEdefs.xml
)

# Concatenate scripts into command: -e script1 [-e script2]
SET(XML_CMD)
FOREACH(script ${XML_BUILD})
  SET(XML_CMD ${XML_CMD} -e ${CMAKE_CURRENT_SOURCE_DIR}/${script})
ENDFOREACH()


# clear lists of generated files
SET(generated_SRC)

# Process each Verilog-A file.
#  * generated files get added to lists (analogfunc, core)
FOREACH( filename ${VA_FILES} )
    # Default
    SET(fileout ${filename}.va)

    # Verilog-A file basename, strip suffix
    GET_FILENAME_COMPONENT(base ${fileout} NAME_WE)
    SET(base_abs ${CMAKE_CURRENT_BINARY_DIR}/${base})

    # set outputs for each Verilog-A input
    SET(output ${base_abs}.analogfunction.cpp
               ${base_abs}.analogfunction.h
               ${base_abs}.cpp
               ${base_abs}.core.cpp
               ${base_abs}.core.h
               ${base_abs}.defs.h)

    # custom command/rule to generate outputs with admsXml
    ADD_CUSTOM_COMMAND(
        OUTPUT ${output}
        COMMAND ${ADMSXML} ${CMAKE_CURRENT_SOURCE_DIR}/${filename}.va ${XML_CMD} -o ${filename}
        DEPENDS ${filename}.va ${XML_BUILD}
        )
    SET_SOURCE_FILES_PROPERTIES(${output} PROPERTIES GENERATED TRUE)

    # append outputs list of generated sources
    SET(generated_SRC ${generated_SRC} ${base_abs}.cpp )
ENDFOREACH()

#MESSAGE(STATUS " ==> generated\n " ${generated_SRC})

ADD_LIBRARY( coreVerilog OBJECT ${generated_SRC} )


# Distribute XML scripts
SET(XML_DIST
  ${XML_BUILD}
  qucsMODULEgui.xml
  qucsMODULEguiJSONsymbol.xml
)

SET(MAKE_FILES
  cpp2lib.makefile
  va2cpp.makefile
)

INSTALL(FILES ${XML_DIST}    DESTINATION include/qucs-core)
INSTALL(FILES ${MAKE_FILES}  DESTINATION include/qucs-core)


