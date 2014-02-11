
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/math
                     ${CMAKE_SOURCE_DIR}/src/components )


SET( INTERFACE_SRC
  qucs_interface.cpp
  e_trsolver.cpp
)

SET(HEADERS
  qucs_interface.h
)

ADD_LIBRARY(interface OBJECT ${INTERFACE_SRC})

ADD_DEPENDENCIES(interface verilog)

INSTALL(FILES ${HEADERS} DESTINATION include/qucs-core)

