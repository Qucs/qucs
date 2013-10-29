
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/math
                     ${CMAKE_SOURCE_DIR}/src/components
                     ${CMAKE_SOURCE_DIR}/src/components/devices )

SET( MICROSTRIP_SRC  substrate.cpp msline.cpp mscorner.cpp msmbend.cpp
	msstep.cpp msopen.cpp msgap.cpp mscoupled.cpp mslange.cpp mstee.cpp mscross.cpp
	msvia.cpp cpwline.cpp cpwopen.cpp cpwshort.cpp cpwgap.cpp cpwstep.cpp
	bondwire.cpp msrstub.cpp )

ADD_LIBRARY(microstrip ${MICROSTRIP_SRC} )
