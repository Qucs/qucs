
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/math
                     ${CMAKE_SOURCE_DIR}/src/components
                     ${CMAKE_SOURCE_DIR}/src/components/devices )

#
# Check if admsXml is available
#  * Use -DADMSXML_DIR=[path] to give the path containing admsXml
#  * Try a few othe locations
FIND_PROGRAM( ADMSXML admsXml
  HINTS ${ADMSXML_DIR}
  PATHS /usr/local/bin/ /opt/local/bin/
  DOC "admsXml application" )

if(ADMSXML)
  message(STATUS "Found admsXml: " ${ADMSXML})
else()
  message(FATAL_ERROR "admsXml was not found!")
endif()

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
  )


# clear lists of generated files
set(analogfunc)
set(core)

# Process each Verilog-A file.
#  * generated files get added to lists (analogfunc, core)
foreach (filename ${VA_FILES})

    # Ugly hack to handle .va files on which the filename differs from module name.
    # The generated files are named after the Verilog-A module's name.

    # Default
    set(fileout ${filename})

    # Special cases
    if(${filename} STREQUAL fbh_hbt-2_2a.va)
      set(fileout HBT_X)
    endif()
    if(${filename} STREQUAL hicumL2V2p11.va)
      set(fileout hicumL2V2p1)
    endif()
    if(${filename} STREQUAL hicumL2V2p22.va)
      set(fileout hic2_full)
    endif()
    if(${filename} STREQUAL hicumL0V1p12.va)
      set(fileout hic0_full)
    endif()

    # Verilog-A file basename, strip suffix
    get_filename_component(base ${fileout} NAME_WE)
    set(base_abs ${CMAKE_CURRENT_BINARY_DIR}/${base})

    # set outputs 1
    set(output ${base_abs}.analogfunction.cpp ${base_abs}.analogfunction.h)

    # custom command/rule to generate outputs with admsXml
    add_custom_command(
        OUTPUT ${output}
        COMMAND ${ADMSXML} ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
                           -e  ${CMAKE_CURRENT_SOURCE_DIR}/analogfunction.xml)
    set_source_files_properties(${output} PROPERTIES GENERATED TRUE)

    # append outputs 1 to global list 'analogfunc'
    set(analogfunc ${analogfunc} ${output} )

    # set outputs 2
    set(output ${base_abs}.core.cpp ${base_abs}.core.h ${base_abs}.defs.h)

    add_custom_command(
        OUTPUT ${output}
        COMMAND ${ADMSXML} ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
                           -e ${CMAKE_CURRENT_SOURCE_DIR}/qucsVersion.xml
                           -e ${CMAKE_CURRENT_SOURCE_DIR}/qucsMODULEcore.xml
                           -e ${CMAKE_CURRENT_SOURCE_DIR}/qucsMODULEdefs.xml )
    set_source_files_properties(${output} PROPERTIES GENERATED TRUE)

    # append outputs 2 to list 'core'
    set(core ${core} ${output} )

    # no need to generate GUI code? Integrated by hand already?
    #set(output ${base_abs}.gui.cpp ${base_abs}.gui.h)
        #COMMAND ${ADMSXML} ${filename} -e qucsVersion.xml -e qucsMODULEgui.xml )
endforeach()

#message(STATUS " ==> analog\n " ${analogfunc})
#message(STATUS " ==> core\n   " ${core})

# create target library out of lists of generated files
add_library(verilog  ${analogfunc} ${core} )

#
# XML_FILES = analogfunction.xml qucsVersion.xml qucsMODULEcore.xml \
#   qucsMODULEdefs.xml qucsMODULEgui.xml
#
# EXTRA_DIST = $(VERILOG_FILES) $(XML_FILES)

