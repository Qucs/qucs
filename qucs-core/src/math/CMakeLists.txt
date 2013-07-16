
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

SET(MATH_SRC
  #cbesselj.cpp
  complex.cpp
  fspecial.cpp
  matrix.cpp
  real.cpp
)

SET(HEADERS
  complex.h
  matrix.h
  precision.h
  real.h
)

ADD_LIBRARY( coreMath OBJECT ${MATH_SRC} )

INSTALL(FILES ${HEADERS} DESTINATION include/qucs-core)


