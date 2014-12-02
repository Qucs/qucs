
INCLUDE_DIRECTORIES( ${qucs-core_SOURCE_DIR}
                     ${qucs-core_CURRENT_SOURCE_DIR}
                     ${qucs-core_SOURCE_DIR}/src/math
                     ${qucs-core_SOURCE_DIR}/src/components   # component.h
                     ${qucs-core_BINARY_DIR}/src/components   # generated verilog/[].core.h
                     )

SET( INTERFACE_SRC
  qucs_interface.cpp
  e_trsolver.cpp
)

SET(HEADERS
  qucs_interface.h
)

ADD_LIBRARY( coreInterface OBJECT ${INTERFACE_SRC})

ADD_DEPENDENCIES( coreInterface coreVerilog)

INSTALL(FILES ${HEADERS} DESTINATION include/qucs-core)

