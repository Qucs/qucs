include(${QucsExamples_SOURCE_DIR}/../cmake/readMakeList.txt)

readVariable(MakeList SRCS SRCS)
readVariable(MakeList CBP CBP)

install(FILES ${SRCS} DESTINATION share/qucs/examples/cpp-interface)
install(FILES ${CBP} DESTINATION share/qucs/examples/cpp-interface)
