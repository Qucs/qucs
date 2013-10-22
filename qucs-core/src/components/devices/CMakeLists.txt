PROJECT(devices CXX)

# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/math \
#  -I$(top_srcdir)/src/components

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ../../..
                     ../../
                     ../
                     ../devices/
                     ../../math )

SET( DEVICES_SRC  device.cpp diode.cpp jfet.cpp bjt.cpp mosfet.cpp
  eqndefined.cpp diac.cpp thyristor.cpp triac.cpp tunneldiode.cpp )



ADD_LIBRARY(devices ${DEVICES_SRC} )





