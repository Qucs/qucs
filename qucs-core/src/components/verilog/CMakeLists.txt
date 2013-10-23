PROJECT(verilog CXX)
cmake_minimum_required(VERSION 2.8)


# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/math \
#  -I$(top_srcdir)/src/components -I$(top_srcdir)/src/components/devices


INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ../../..
                     ../../
                     ../
                     ../devices/
                     ../../math )

# apart from the commented names, all is derived from
# the va file itself.

SET( VA_FILES
#  constants.vams   #--> created by admsXml
#  disciplines.vams #--> created by admsXml
  #fbh_hbt-2_2a.va  #--> XBT_X
  #hicumL2V2p11.va  #--> hicumL2V2p1
  mod_amp.va
  #hicumL2V2p22.va  #--> hic2_full
  log_amp.va
  #hicumL0V1p12.va  #--> hic0_full
  potentiometer.va MESFET.va hicumL0V1p2.va hicumL2V2p23.va
  photodiode.va EKV26MOS.va bsim3v34nMOS.va  bsim3v34pMOS.va bsim4v30nMOS.va bsim4v30pMOS.va
  hicumL2V2p24.va hicumL0V1p2g.va hicumL0V1p3.va hicumL2V2p31n.va
  phototransistor.va nigbt.va
  dff_SR.va jkff_SR.va tff_SR.va gatedDlatch.va logic_0.va logic_1.va
  mux2to1.va mux4to1.va mux8to1.va DLS_nto1.va DLS_1ton.va andor4x2.va
  andor4x3.va andor4x4.va dmux2to4.va dmux3to8.va dmux4to16.va ha1b.va
  fa1b.va fa2b.va pad2bit.va pad3bit.va pad4bit.va binarytogrey4bit.va
  greytobinary4bit.va comp_1bit.va comp_2bit.va comp_4bit.va hpribin4bit.va )


# find and assing admsXml to variable
FIND_PROGRAM( ADMSXML admsXml
  PATHS /usr/local/bin/ /usr/local/Cellar/adms/HEAD/bin/
  DOC "admsXml application" )


message(STATUS " ==> found admsXml " ${ADMSXML})


#http://stackoverflow.com/questions/2687136/how-do-i-make-build-rules-in-cmake-to-preprocess-lazy-c-lzz-files-that-genera

#function(lazy_compile2 filename)
#    get_filename_component(base ${filename} NAME_WE)
#    set(base_abs ${CMAKE_CURRENT_BINARY_DIR}/${base})
#    set(output ${base_abs}.core.cpp ${base_abs}.core.h)
#    add_custom_command(
#        OUTPUT ${output}
#        COMMAND ${ADMSXML} ${filename} -e qucsVersion.xml -e qucsMODULEcore.xml )
#    set_source_files_properties(${output} PROPERTIES GENERATED TRUE)
#set( result ${result} ${output})
#  message(STATUS "${result}")
#endfunction()


# TODO, if VA files change, should recompile

# process Verilog-A file and append generated files
# into global 'result' list
function(adms_compile filename)
    # basename
    get_filename_component(base ${filename} NAME_WE)
    set(base_abs ${CMAKE_CURRENT_BINARY_DIR}/${base})

    # list output names
    set(output ${base_abs}.analogfunction.cpp ${base_abs}.analogfunction.h)
    # custom command/rule to generate outputs with admsXml
    add_custom_command(
        OUTPUT ${output}
        COMMAND ${ADMSXML} ${CMAKE_CURRENT_SOURCE_DIR}/${filename} -e  ${CMAKE_CURRENT_SOURCE_DIR}/analogfunction.xml)
    set_source_files_properties(${output} PROPERTIES GENERATED TRUE)
    # append outputs to global list/scope
    set(analogfunc ${analogfunc} ${output} PARENT_SCOPE)


    set(output ${base_abs}.core.cpp ${base_abs}.core.h)

    #set_source_files_properties(${output} properties object_depends ${filename})
    #set_source_files_properties(${filename} properties object_outputs ${output})

#message(STATUS " ==> core\n\n\n   " ${filename})

    add_custom_command(
        OUTPUT ${output}
        COMMAND ${ADMSXML} ${CMAKE_CURRENT_SOURCE_DIR}/${filename} -e ${CMAKE_CURRENT_SOURCE_DIR}/qucsVersion.xml -e ${CMAKE_CURRENT_SOURCE_DIR}/qucsMODULEcore.xml )
        #depends ${filename}) invalid?
#        [ 10%] Generating mod_amp.core.cpp, mod_amp.core.h
#cd /Users/guilherme/git/qucs/qucs-core/build/src/components/verilog && /usr/local/bin/admsXml /Users/guilherme/git/qucs/qucs-core/src/components/verilog/mod_amp.va -e /Users/guilherme/git/qucs/qucs-core/src/components/verilog/qucsVersion.xml -e /Users/guilherme/git/qucs/qucs-core/src/components/verilog/qucsMODULEcore.xml depends mod_amp.va
#[fatal..] option -f: cannot read file './depends'
    set_source_files_properties(${output} PROPERTIES GENERATED TRUE)
    set(core ${core} ${output} PARENT_SCOPE)


    set(output ${base_abs}.defs.h)
    add_custom_command(
        OUTPUT ${output}
        COMMAND ${ADMSXML} ${CMAKE_CURRENT_SOURCE_DIR}/${filename} -e ${CMAKE_CURRENT_SOURCE_DIR}/qucsVersion.xml -e ${CMAKE_CURRENT_SOURCE_DIR}/qucsMODULEdefs.xml )
    set_source_files_properties(${output} PROPERTIES GENERATED TRUE)
    set(defs ${defs} ${output} PARENT_SCOPE)

    # no need to generate GUI code? Integrated by hand already?
    #set(output ${base_abs}.gui.cpp ${base_abs}.gui.h)
        #COMMAND ${ADMSXML} ${filename} -e qucsVersion.xml -e qucsMODULEgui.xml )
endfunction()


# clear list
set(analogfunc)
set(core)
set(defs) # only def.h files, it did't work if appened with core.{cpp,h}

# process VA files and append generated to list
foreach (_va ${VA_FILES})
  adms_compile(${_va})
endforeach()

#message(STATUS " ==> analog\n " ${analogfunc})
message(STATUS " ==> core\n   " ${core})
#message(STATUS " ==> defs\n   " ${defs})


# create target library out of generated files
add_library(verilog  ${analogfunc} ${core} ${defs} ${result} ${heads})

#SET_TARGET_PROPERTIES(verilog PROPERTIES LINKER_LANGUAGE CXX)

#
#if MAINTAINER_MODE
### configure determines if we want to build and use the
### bundled version of ADMS, or the host's ADMS. Here we
### set the path to the admsXml binary depeding on the choice
### the user made at configure time
#if USE_INTERNAL_ADMS
#ADMSXML = $(abs_top_builddir)/adms/admsXml/admsXml
#else
#ADMSXML = @ADMSXML@
#endif
#
#
#nigbt.analogfunction.cpp: analogfunction.xml
#nigbt.analogfunction.cpp: nigbt.va
#	$(ADMSXML) $< -e $(srcdir)/analogfunction.xml
#
#nigbt.core.cpp: nigbt.defs.h qucsVersion.xml qucsMODULEcore.xml
#nigbt.core.cpp: nigbt.va
#	$(ADMSXML) $< -e $(srcdir)/qucsVersion.xml -e $(srcdir)/qucsMODULEcore.xml
#
#nigbt.defs.h: qucsVersion.xml qucsMODULEdefs.xml
#nigbt.defs.h: nigbt.va
#	$(ADMSXML) $< -e $(srcdir)/qucsVersion.xml -e $(srcdir)/qucsMODULEdefs.xml
#
#nigbt.gui.cpp: qucsVersion.xml qucsMODULEgui.xml
#nigbt.gui.cpp: nigbt.va
#	$(ADMSXML) $< -e $(srcdir)/qucsVersion.xml -e $(srcdir)/qucsMODULEgui.xml
#
#
#
#
#ADD_LIBRARY(verilog $VERILOG_SRC} )
