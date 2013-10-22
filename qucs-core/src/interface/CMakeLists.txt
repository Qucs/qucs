PROJECT(interface CXX)

# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/math
                     ${CMAKE_SOURCE_DIR}/src/components )

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/math -I$(top_srcdir)/src/components


SET(INTERFACE_SRC qucs_interface.cpp e_trsolver.cpp)

ADD_LIBRARY(interface ${INTERFACE_SRC})

