
INCLUDE_DIRECTORIES( ${qucs-core_SOURCE_DIR}
                     ${qucs-core_CURRENT_SOURCE_DIR}
                     ${qucs-core_SOURCE_DIR}/src/math
                     ${qucs-core_SOURCE_DIR}/src/components             # component.h
                     ${qucs-core_SOURCE_DIR}/src/components/devices )   # devices.h

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

ADD_LIBRARY( coreMicrostrip OBJECT ${MICROSTRIP_SRC} )

