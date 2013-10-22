PROJECT(microstrip CXX)

# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/math \
#  -I$(top_srcdir)/src/components -I$(top_srcdir)/src/components/devices

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ../../..
                     ../../
                     ../
                     ../devices/
                     ../../math )


SET( MICROSTRIP_SRC  substrate.cpp msline.cpp mscorner.cpp msmbend.cpp
	msstep.cpp msopen.cpp msgap.cpp mscoupled.cpp mslange.cpp mstee.cpp mscross.cpp
	msvia.cpp cpwline.cpp cpwopen.cpp cpwshort.cpp cpwgap.cpp cpwstep.cpp
	bondwire.cpp msrstub.cpp )


ADD_LIBRARY(microstrip ${MICROSTRIP_SRC} )

#ok

