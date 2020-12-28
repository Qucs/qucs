project(qucsfilter CXX C)
cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)

# use top VERSION file
file(STRINGS ${PROJECT_SOURCE_DIR}/../VERSION QUCS_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${QUCS_VERSION}")

set(PROJECT_VERSION "${QUCS_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2014")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DHAVE_CONFIG_H)

# configure the header config.h
configure_file("${PROJECT_SOURCE_DIR}/../config.h.cmake"
               "${PROJECT_BINARY_DIR}/config.h")

include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") # enable warning level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ") # enable C++11

# flag not available in mingw 4.8.2, MSVC10
if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register ")
endif()

find_package(Qt4 REQUIRED)
set(QT_USE_QTGUI TRUE)
set(QT_USE_QTSVG TRUE)

include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})

set(QUCS-FILTER_SRCS
    cline_filter.cpp
    eqn_filter.cpp
    filter.cpp
    helpdialog.cpp
    lc_filter.cpp
    line_filter.cpp
    main.cpp
    qf_cauer.cpp
    qf_filter.cpp
    qf_poly.cpp
    qucsfilter.cpp
    stepz_filter.cpp
    tl_filter.cpp
    quarterwave_filter.cpp
    qw_coupled_ring_filter.cpp
    ccoupled_shunt_resonators.cpp)

set(QUCS-FILTER_HDRS
    cline_filter.h
    eqn_filter.h
    filter.h
    lc_filter.h
    line_filter.h
    material_props.h
    qf_cauer.h
    qf_filter.h
    qf_matrix.h
    qf_poly.h
    stepz_filter.h
    tl_filter.h
    quarterwave_filter.h)

set(QUCS-FILTER_MOC_HDRS helpdialog.h qucsfilter.h)

qt4_wrap_cpp(QUCS-FILTER_MOC_SRCS ${QUCS-FILTER_MOC_HDRS})

set(RESOURCES qucsfilter_.qrc)
qt4_add_resources(RESOURCES_SRCS ${RESOURCES})

if(APPLE)
  # set information on Info.plist file
  set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER
      "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
  set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
  set(MACOSX_BUNDLE_ICON_FILE qucsfilter.icns)

  # set where in the bundle to put the icns file
  set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucsfilter.icns
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  set(QUCS-FILTER_SRCS
      ${QUCS-FILTER_SRCS}
      ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucsfilter.icns)

endif(APPLE)

add_executable(
  qucsfilter MACOSX_BUNDLE WIN32 ${QUCS-FILTER_SRCS} ${QUCS-FILTER_HDRS}
                                 ${QUCS-FILTER_MOC_SRCS} ${RESOURCES_SRCS})

target_link_libraries(qucsfilter ${QT_LIBRARIES})

install(
  TARGETS qucsfilter
  BUNDLE DESTINATION bin COMPONENT Runtime
  RUNTIME DESTINATION bin COMPONENT Runtime)

# man pages
install(FILES qucsfilter.1 DESTINATION share/man/man1)
