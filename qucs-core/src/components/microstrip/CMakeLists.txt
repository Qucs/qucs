
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/math
                     ${CMAKE_SOURCE_DIR}/src/components
                     ${CMAKE_SOURCE_DIR}/src/components/devices )

SET( MICROSTRIP_SRC
  bondwire.cpp
  cpwgap.cpp
  cpwline.cpp
  cpwopen.cpp
  cpwshort.cpp
  cpwstep.cpp
  mscorner.cpp
  mscoupled.cpp
  mscross.cpp
  msgap.cpp
  mslange.cpp
  msline.cpp
  msmbend.cpp
  msopen.cpp
  msrstub.cpp
  msstep.cpp
  mstee.cpp
  msvia.cpp
  substrate.cpp
)

ADD_LIBRARY(microstrip OBJECT ${MICROSTRIP_SRC} )

