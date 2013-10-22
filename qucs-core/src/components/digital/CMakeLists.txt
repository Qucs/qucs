PROJECT(digital CXX)

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

SET( DIGITAL_SRC digital.cpp inverter.cpp nor.cpp or.cpp nand.cpp
  and.cpp xnor.cpp xor.cpp digisource.cpp buffer.cpp )


ADD_LIBRARY(digital ${DIGITAL_SRC} )




