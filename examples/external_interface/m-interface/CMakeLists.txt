include(${QucsExamples_SOURCE_DIR}/../cmake/readMakeList.txt)

readVariable(MakeList M_FILES M_FILES)
readVariable(MakeList NETLIST_FILE NETLIST_FILE)

install(FILES ${M_FILES} DESTINATION share/qucs/examples/m-interface)
install(FILES ${NETLIST_FILE} DESTINATION share/qucs/examples/m-interface)
