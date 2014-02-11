
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/math
                     ${CMAKE_SOURCE_DIR}/src/components/devices )

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

ADD_LIBRARY(devices OBJECT ${DEVICES_SRC} )

SET(HEADERS
  device.h
)

INSTALL(FILES ${HEADERS} DESTINATION include/qucs-core)

