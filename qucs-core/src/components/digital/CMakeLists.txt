
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/math
                     ${CMAKE_SOURCE_DIR}/src/components/devices )

SET( DIGITAL_SRC
  and.cpp
  buffer.cpp
  digisource.cpp
  digital.cpp
  inverter.cpp
  nand.cpp
  nor.cpp
  or.cpp
  xnor.cpp
  xor.cpp
)

ADD_LIBRARY(digital OBJECT ${DIGITAL_SRC} )

