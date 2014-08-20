
INCLUDE_DIRECTORIES( ${qucs-core_SOURCE_DIR}
                     ${qucs-core_CURRENT_SOURCE_DIR}
                     ${qucs-core_SOURCE_DIR}/src/math
                     ${qucs-core_SOURCE_DIR}/src/components ) # component.h

SET( DEVICES_SRC
  bjt.cpp
  device.cpp
  diac.cpp
  diode.cpp
  eqndefined.cpp
  jfet.cpp
  mosfet.cpp
  thyristor.cpp
  triac.cpp
  tunneldiode.cpp
)

ADD_LIBRARY( coreDevices OBJECT ${DEVICES_SRC} )

SET(HEADERS
  device.h
)

INSTALL(FILES ${HEADERS} DESTINATION include/qucs-core)

