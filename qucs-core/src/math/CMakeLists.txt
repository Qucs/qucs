
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

SET(MATH_SRC
  cbesselj.cpp
  complex.cpp
  fspecial.cpp
  matrix.cpp
  precision.c
  real.cpp
)

SET(HEADERS
  precision.h
)

ADD_LIBRARY(math OBJECT ${MATH_SRC} )

INSTALL(FILES ${HEADERS} DESTINATION include/qucs-core)


