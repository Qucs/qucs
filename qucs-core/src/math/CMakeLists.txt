
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

SET(QUCSMATH_SRC complex.cpp cmplx.cpp fspecial.cpp real.cpp cbesselj.cpp precision.c)

ADD_LIBRARY(qucsmath ${QUCSMATH_SRC} )
