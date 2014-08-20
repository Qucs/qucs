
INCLUDE_DIRECTORIES( ${qucs-core_SOURCE_DIR}
                     ${qucs-core_CURRENT_SOURCE_DIR}
                     ${qucs-core_SOURCE_DIR}/src/math
                     ${qucs-core_SOURCE_DIR}/src/components             # component.h
                     ${qucs-core_SOURCE_DIR}/src/components/devices )   # devices.h


# TODO does constants / disciplines need to be generated, or adms creates them as needed?
# TODO, test if VA files change, should recompile

SET( VA_FILES
#  constants.vams   #--> created by admsXml if not found
#  disciplines.vams #--> created by admsXml
  andor4x2.va
  andor4x3.va
  andor4x4.va
  binarytogrey4bit.va
  bsim3v34nMOS.va
  bsim3v34pMOS.va
  bsim4v30nMOS.va
  bsim4v30pMOS.va
  comp_1bit.va
  comp_2bit.va
  comp_4bit.va
  dff_SR.va
  DLS_1ton.va
  DLS_nto1.va
  dmux2to4.va
  dmux3to8.va
  dmux4to16.va
  EKV26MOS.va
  fa1b.va
  fa2b.va
  fbh_hbt-2_2a.va  #--> module HBT_X
  gatedDlatch.va
  greytobinary4bit.va
  ha1b.va
  hicumL0V1p12.va  #--> module hic0_full
  hicumL0V1p2.va
  hicumL0V1p2g.va
  hicumL0V1p3.va
  hicumL2V2p11.va  #--> module hicumL2V2p1
  hicumL2V2p22.va  #--> module hic2_full
  hicumL2V2p23.va
  hicumL2V2p24.va
  hicumL2V2p31n.va
  hpribin4bit.va
  jkff_SR.va
  log_amp.va
  logic_0.va
  logic_1.va
  MESFET.va
  mod_amp.va
  mux2to1.va
  mux4to1.va
  mux8to1.va
  nigbt.va
  pad2bit.va
  pad3bit.va
  pad4bit.va
  photodiode.va
  phototransistor.va
  potentiometer.va
  tff_SR.va
  vcresistor.va
)


# clear lists of generated files
SET(generated_SRC)

# Process each Verilog-A file.
#  * generated files get added to lists (analogfunc, core)
FOREACH( filename ${VA_FILES} )

    # Ugly hack to handle .va files on which the filename differs from module name.
    # The generated files are named after the Verilog-A module's name.

    # Default
    SET(fileout ${filename})

    # Special cases
    IF(${filename} STREQUAL fbh_hbt-2_2a.va)
      SET(fileout HBT_X)
    ENDIF()
    IF(${filename} STREQUAL hicumL2V2p11.va)
      SET(fileout hicumL2V2p1)
    ENDIF()
    IF(${filename} STREQUAL hicumL2V2p22.va)
      SET(fileout hic2_full)
    ENDIF()
    IF(${filename} STREQUAL hicumL0V1p12.va)
      SET(fileout hic0_full)
    ENDIF()

    # Verilog-A file basename, strip suffix
    GET_FILENAME_COMPONENT(base ${fileout} NAME_WE)
    SET(base_abs ${CMAKE_CURRENT_BINARY_DIR}/${base})

    # set outputs for each Verilog-A input
    SET(output ${base_abs}.analogfunction.cpp ${base_abs}.analogfunction.h
               ${base_abs}.core.cpp ${base_abs}.core.h ${base_abs}.defs.h)

    # custom command/rule to generate outputs with admsXml
    ADD_CUSTOM_COMMAND(
        OUTPUT ${output}
        COMMAND ${ADMSXML} ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
                           -e ${CMAKE_CURRENT_SOURCE_DIR}/analogfunction.xml
                           -e ${CMAKE_CURRENT_SOURCE_DIR}/qucsVersion.xml
                           -e ${CMAKE_CURRENT_SOURCE_DIR}/qucsMODULEcore.xml
                           -e ${CMAKE_CURRENT_SOURCE_DIR}/qucsMODULEdefs.xml )
    SET_SOURCE_FILES_PROPERTIES(${output} PROPERTIES GENERATED TRUE)

    # append outputs list of generated sources
    SET(generated_SRC ${generated_SRC} ${output} )

    # no need to generate GUI code? Integrated manually already?
    #set(output ${base_abs}.gui.cpp ${base_abs}.gui.h)
        #COMMAND ${ADMSXML} ${filename} -e qucsVersion.xml -e qucsMODULEgui.xml )
ENDFOREACH()

#MESSAGE(STATUS " ==> generated\n " ${generated_SRC})

ADD_LIBRARY( coreVerilog OBJECT ${generated_SRC} )


# TODO install distributable files
#
#
# EXTRA_DIST = $(VERILOG_FILES) $(XML_FILES)

SET(XML_FILES
  analogfunction.xml
  qucsMODULEcore.xml
  qucsMODULEdefs.xml
  qucsMODULEgui.xml
  qucsMODULEguiJSONsymbol.xml
  qucsVersion.xml
)

SET(MAKE_FILES
  cpp2lib.makefile
  va2cpp.makefile
)

SET(VA_INCLUDES
  constants.vams
  disciplines.vams
)

INSTALL(FILES ${XML_FILES}   DESTINATION include/qucs-core)
INSTALL(FILES ${MAKE_FILES}  DESTINATION include/qucs-core)
INSTALL(FILES ${VA_INCLUDES} DESTINATION include/qucs-core)


