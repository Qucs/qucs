
INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR} )

# FIXME: not in use, config.h is included directly no conditional
ADD_DEFINITIONS(-DHAVE_CONFIG_H)


# Maintainer mode runs the Perl scrips that generate source code
if ( USE_MAINTAINER_MODE )

    # Run mkelements.pl
    #   Read in `adms.xml` and generete portions of adms and admst
    # Generate outputs:
    #    adms.h
    #    adms.c
    #    admstpathYacc.y
    #    admstpathYacc.h
    #    admstpath.dtd
    MESSAGE(STATUS "Running mkelements.pl code generator")

    EXECUTE_PROCESS(COMMAND
                    perl ${PROJECT_SOURCE_DIR}/admsXml/mkelements.pl ${PROJECT_SOURCE_DIR}
                    RESULT_VARIABLE element_result )

    IF(element_result EQUAL 0)
      MESSAGE(STATUS "mkelement.pl completed successfully")
    ELSE(element_result EQUAL 0)
      MESSAGE(FATAL_ERROR "mkelement.pl failed. Exit code: ${element_result}")
    ENDIF(element_result EQUAL 0)


    # Run mkctext.pl
    #   Wrap source files as C code
    # Generate outputs:
    #    constants.vams.c
    #    disciplines.vams.c
    #    adms.implicit.xml.c
    #    dummy.va.c
    #    dummy.xml.c
    SET(CTEXT
        constants.vams
        disciplines.vams
        adms.implicit.xml
        dummy.va
        dummy.xml )

    FOREACH( inc ${CTEXT} )
      MESSAGE(STATUS "Running mkctext.pl code generator on ${inc}")

      EXECUTE_PROCESS( COMMAND
                       perl ${PROJECT_SOURCE_DIR}/admsXml/mkctext.pl ${PROJECT_SOURCE_DIR}/admsXml/${inc} ${PROJECT_SOURCE_DIR}/admsXml
                       RESULT_VARIABLE text_result )

      IF(text_result EQUAL 0)
        MESSAGE(STATUS "mkctext.pl completed successfully")
      ELSE(text_result EQUAL 0)
        MESSAGE(FATAL_ERROR "mkctext.pl failed. Exit code: ${text_result}")
      ENDIF(text_result EQUAL 0)
    ENDFOREACH()


    # Run mkgrammar.pl
    #   Process verilogaYacc.y.in and generate the parser
    # Generate output:
    #    verilogaYacc.y
    #
    MESSAGE(STATUS "Running mkgrammar.pl code generator")

    EXECUTE_PROCESS(COMMAND perl ${PROJECT_SOURCE_DIR}/admsXml/mkgrammar.pl ${PROJECT_SOURCE_DIR}/admsXml/verilogaYacc.y.in
                    RESULT_VARIABLE grammar_result )

    IF(grammar_result EQUAL 0)
      MESSAGE(STATUS "mkgrammar.pl completed successfully")
    ELSE(grammar_result EQUAL 0)
      MESSAGE(FATAL_ERROR "mkgrammar.pl failed. Exit code: ${grammar_result}")
    ENDIF(grammar_result EQUAL 0)

ENDIF ( USE_MAINTAINER_MODE )

#
# -= admsElement =-
#
ADD_LIBRARY(admsElement adms.c)

# mkctext.pl -> dummy.va.c ends up on the top dir with in maintainer mode
include_directories(${CMAKE_BINARY_DIR} )

#
# -= admsPreprocessor =-
#
BISON_TARGET(preproParse ${CMAKE_CURRENT_SOURCE_DIR}/preprocessorYacc.y ${CMAKE_CURRENT_SOURCE_DIR}/preprocessorYacc.c )
FLEX_TARGET(preproScan ${CMAKE_CURRENT_SOURCE_DIR}/preprocessorLex.l  ${CMAKE_CURRENT_SOURCE_DIR}/preprocessorLex.c )
ADD_FLEX_BISON_DEPENDENCY(preproScan preproParse)

#MESSAGE(STATUS "Generate ${BISON_preproParse_OUTPUTS} ${FLEX_preoproScan_OUTPUTS} ")

ADD_LIBRARY(admsPreprocessor preprocessorMain.c
            ${BISON_preproParse_OUTPUTS} ${FLEX_preproScan_OUTPUTS} )

#
# -= admsVeriloga =-
#
# NOTE: could not get the generated `verilogaYacc.y` to end up in SOURCE_DIR/admsXml/, it ends up on BINARY_DIR
# As a workaroud, we check for the existence of the generated file. In USE_MAINTAINER_MODE, CMake generated takes precende
if(EXISTS "${CMAKE_BINARY_DIR}/verilogaYacc.y")
  # generated with CMake
  BISON_TARGET(vaParse ${CMAKE_BINARY_DIR}/verilogaYacc.y ${CMAKE_CURRENT_SOURCE_DIR}/verilogaYacc.c )
else()
  # generated with AutoTools (release tarball)
  BISON_TARGET(vaParse ${CMAKE_CURRENT_SOURCE_DIR}/verilogaYacc.y ${CMAKE_CURRENT_SOURCE_DIR}/verilogaYacc.c )
endif()
FLEX_TARGET(vaScan ${CMAKE_CURRENT_SOURCE_DIR}/verilogaLex.l  ${CMAKE_CURRENT_SOURCE_DIR}/verilogaLex.c )
ADD_FLEX_BISON_DEPENDENCY(vaScan vaParse)

#MESSAGE(STATUS "Generate ${BISON_vaParse_OUTPUTS} ${FLEX_vaScan_OUTPUTS} ")

ADD_LIBRARY(admsVeriloga
            ${FLEX_vaScan_OUTPUTS} ${BISON_vaParse_OUTPUTS} )

#
# -= admsAdmstpath =-
#
ADD_LIBRARY(admsAdmstpath admstpathYacc.c)
SET_SOURCE_FILES_PROPERTIES(admstpathYacc.c admstpathYacc.h admstpathLex.c PROPERTIES GENERATED TRUE)
SET_TARGET_PROPERTIES(admsAdmstpath PROPERTIES COMPILE_DEFINITIONS insideadmstpathYacc)
ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/admstpathYacc.c ${CMAKE_CURRENT_SOURCE_DIR}/admstpathYacc.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${BISON_EXECUTABLE}
    #ARGS -by ${BISON_APIPREFIX}admstpath -oadmstpathYacc.c admstpathYacc.y
    ARGS -by -oadmstpathYacc.c admstpathYacc.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/admstpathYacc.y
)

#
# -= admsXml =-
#
ADD_EXECUTABLE(admsXml admsXml.c)
TARGET_LINK_LIBRARIES(admsXml admsElement admsVeriloga admsPreprocessor admsAdmstpath -lm)


#
# -= admsCheck =-
#
ADD_EXECUTABLE(admsCheck admsCheck.c)
TARGET_LINK_LIBRARIES(admsCheck admsElement)

#
# Handle installation
#
INSTALL(TARGETS admsXml DESTINATION bin)
INSTALL(TARGETS admsCheck DESTINATION bin)

INSTALL(FILES admsXml.1 DESTINATION share/man/man1)
INSTALL(FILES admsCheck.1 DESTINATION share/man/man1)
