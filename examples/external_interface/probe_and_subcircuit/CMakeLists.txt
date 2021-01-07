include(${QucsExamples_SOURCE_DIR}/../cmake/readMakeList.txt)

readVariable(MakeList SCHEMATICS SCHEMATICS)

install(FILES ${SCHEMATICS} DESTINATION share/qucs/examples/probe_and_subcircuit)
