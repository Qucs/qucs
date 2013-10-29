
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/math
                     ${CMAKE_SOURCE_DIR}/src/components/devices )

SET( DIGITAL_SRC digital.cpp inverter.cpp nor.cpp or.cpp nand.cpp
  and.cpp xnor.cpp xor.cpp digisource.cpp buffer.cpp )

ADD_LIBRARY(digital ${DIGITAL_SRC} )
