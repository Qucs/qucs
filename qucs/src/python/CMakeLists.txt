# python scripts
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/readMakeList.txt)

readVariable(MakeList BASICS BASICS_)

install(FILES ${BASICS_} DESTINATION share/qucs/python/)

# TODO: also qucs-py???
