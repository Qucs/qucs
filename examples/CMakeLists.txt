project(QucsExamples)

include(../cmake/readMakeList.txt)

readVariable(MakeList SCHEMATICS SCHEMATICS)
install(FILES ${SCHEMATICS} DESTINATION share/qucs/examples)

add_subdirectory(external_interface)
