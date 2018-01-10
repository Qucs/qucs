PROJECT(qucsfilter-v2 CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
cmake_policy(VERSION 2.6)

SET(CMAKE_BUILD_TYPE Debug)

# The incomplete filter-v2 only builds withs -stdlib=libstc++
# OSX defaults to libstc++. \fixme
IF(APPLE)
  SET(CMAKE_CXX_FLAGS  "-stdlib=libstdc++")
ENDIF()


ADD_DEFINITIONS( -DHAVE_CONFIG_H )

# configure the header config.h
CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")


include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


#
# Set position independed code PIC
#
IF (UNIX AND NOT APPLE)
  IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
  ENDIF()
ENDIF()


IF(${USE_QT_VERSION} MATCHES "4")
    FIND_PACKAGE( Qt4 REQUIRED )
    SET( QT_USE_QTGUI TRUE )
    SET( QT_USE_QTXML TRUE )
#     SET( QT_USE_QT3SUPPORT TRUE )

    INCLUDE( ${QT_USE_FILE} )

#     ADD_DEFINITIONS(${QT_DEFINITIONS} )
    
    IF(DISABLE_QT3_SUPPORT)
        message(STATUS "disable qt3 support classes(): ${DISABLE_QT3_SUPPORT}")
        ADD_DEFINITIONS(${QT_DEFINITIONS} -DNO_QT3SUPPORT -DDISABLE_QT3SUPPORT)
    ELSE()
        ADD_DEFINITIONS(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS -DQT3_SUPPORT_WARNINGS)
    ENDIF()
ELSE()
    FIND_PACKAGE( Qt5 COMPONENTS Core Gui Xml REQUIRED QUIET)
    INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS})
    
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    ADD_DEFINITIONS(${QT_DEFINITIONS})
ENDIF()

IF(DISABLE_QDEBUG)
    message(STATUS "disable qDebug(): ${DISABLE_QDEBUG}")
    ADD_DEFINITIONS(-DQT_NO_DEBUG_OUTPUT -DQT_NO_DEBUG -DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
ENDIF()



SET(QUCS-FILTER2_SRCS
qf_api.cpp
qf_bessel.cpp
qf_blinch.cpp
qf_box.cpp
qf_butcheb.cpp
qf_cauer.cpp
qf_common.cpp
qf_comp.cpp
qf_dialog.cpp
qf_filter.cpp
qf_icheb.cpp
qf_legendre.cpp
qf_main.cpp
qf_matrix.cpp
qf_optimize.cpp
qf_poly.cpp
qf_qr.cpp
qf_settings.cpp
qf_tform.cpp
#qf_transform.cpp
qf_zigzag.cpp
)

SET(QUCS-FILTER2_MOC_HDRS
qf_box.h
qf_dialog.h
)

IF(${USE_QT_VERSION} MATCHES "4")
    QT4_WRAP_CPP(QUCS-FILTER2_MOC_SRCS ${QUCS-FILTER2_MOC_HDRS})
ELSE()
    QT5_WRAP_CPP(QUCS-FILTER2_MOC_SRCS ${QUCS-FILTER2_MOC_HDRS})
ENDIF()

ADD_EXECUTABLE(qucs-filter-v2 ${QUCS-FILTER2_SRCS} ${QUCS-FILTER2_MOC_SRCS})


IF(${USE_QT_VERSION} MATCHES "4")
    TARGET_LINK_LIBRARIES(qucs-filter-v2 ${QT_LIBRARIES})
ELSE()
    TARGET_LINK_LIBRARIES(qucs-filter-v2 ${QT_LIBRARIES} ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Xml_LIBRARIES} )
ENDIF()

INSTALL(TARGETS qucs-filter-v2 DESTINATION bin)


