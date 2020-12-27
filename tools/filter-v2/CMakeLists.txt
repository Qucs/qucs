project(qucsfilter-v2 CXX C)
cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)

# The incomplete filter-v2 only builds withs -stdlib=libstc++ OSX defaults to
# libstc++. \fixme
if(APPLE)
  set(CMAKE_CXX_FLAGS "-stdlib=libstdc++")
endif()

add_definitions(-DHAVE_CONFIG_H)

# configure the header config.h
configure_file("${PROJECT_SOURCE_DIR}/../config.h.cmake"
               "${PROJECT_BINARY_DIR}/config.h")

include_directories("${PROJECT_BINARY_DIR}")

find_package(Qt4 REQUIRED)
set(QT_USE_QTGUI TRUE)
set(QT_USE_QTXML TRUE)
set(QT_USE_QT3SUPPORT TRUE)

include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})

set(QUCS-FILTER2_SRCS
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
    # qf_transform.cpp
    qf_zigzag.cpp)

set(QUCS-FILTER2_MOC_HDRS qf_box.h qf_dialog.h)

qt4_wrap_cpp(QUCS-FILTER2_MOC_SRCS ${QUCS-FILTER2_MOC_HDRS})

add_executable(qucs-filter-v2 ${QUCS-FILTER2_SRCS} ${QUCS-FILTER2_MOC_SRCS})

target_link_libraries(qucs-filter-v2 ${QT_LIBRARIES})

install(TARGETS qucs-filter-v2 DESTINATION bin)
