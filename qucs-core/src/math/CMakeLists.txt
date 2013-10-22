
# defines nr_double_t
ADD_DEFINITIONS( -DHAVE_CONFIG_H )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}
                     ../.. # qucs-core
                     ..)   # qucs-core/src

SET(QUCSMATH_SRC complex.cpp cmplx.cpp fspecial.cpp real.cpp cbesselj.cpp precision.c)

ADD_LIBRARY(qucsmath ${QUCSMATH_SRC} )

# ok!
