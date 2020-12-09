# python scripts
include(../../../cmake/readMakeList.txt)

readVariable(MakeList BASICS BASICS_)

install(FILES ${BASICS_} DESTINATION share/qucs/python/)

# TODO: also qucs-py???
