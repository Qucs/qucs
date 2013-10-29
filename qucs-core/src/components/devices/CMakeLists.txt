
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/math
                     ${CMAKE_SOURCE_DIR}/src/components/devices )

SET( DEVICES_SRC  device.cpp diode.cpp jfet.cpp bjt.cpp mosfet.cpp
  eqndefined.cpp diac.cpp thyristor.cpp triac.cpp tunneldiode.cpp )

ADD_LIBRARY(devices ${DEVICES_SRC} )
