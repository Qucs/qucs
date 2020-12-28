# octave scripts
include(../../cmake/readMakeList.txt)

readVariable(MakeList BASICS BASICS_)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${BASICS_} DESTINATION share/qucs/octave/)
