project(qucspowercombining CXX C)
cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)
find_package(Qt4 COMPONENTS QtSvg QtGui QtCore)
include(${QT_USE_FILE})
# use top VERSION file
file(STRINGS ${PROJECT_SOURCE_DIR}/../VERSION QUCS_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${QUCS_VERSION}")

set(PROJECT_VERSION "${QUCS_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2016")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DHAVE_CONFIG_H)

# define variables
set(BINARYDIR "${CMAKE_INSTALL_PREFIX}/bin/")
set(BITMAPDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/bitmaps/")
set(DOCDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/docs/")
set(LANGUAGEDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/lang/")
set(LIBRARYDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/library/")
set(OCTAVEDIR "${CMAKE_INSTALL_PREFIX}/share/qucs/octave/")

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

include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})

set(QUCS-POWCOMB_SRCS qucspowercombiningtool.cpp main.cpp)

set(QUCS-POWCOMB_HDRS qucspowercombiningtool.h)

set(QUCS-POWCOMB_MOC_HDRS qucspowercombiningtool.h)

qt4_wrap_cpp(QUCS-POWCOMB_MOC_SRCS ${QUCS-POWCOMB_MOC_HDRS})

set(RESOURCES qucspowercombining.qrc)
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
  set(MACOSX_BUNDLE_ICON_FILE qucspowercombining.icns)

  # set where in the bundle to put the icns file
  set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucspowercombining.icns
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  set(QUCS-POWCOMB_SRCS
      ${QUCS-POWCOMB_SRCS}
      ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucspowercombining.icns)

endif(APPLE)

add_executable(
  qucspowercombining MACOSX_BUNDLE WIN32
  ${QUCS-POWCOMB_SRCS} ${QUCS-POWCOMB_HDRS} ${QUCS-POWCOMB_MOC_SRCS}
  ${RESOURCES_SRCS})

target_link_libraries(qucspowercombining ${QT_LIBRARIES})

install(
  TARGETS qucspowercombining
  BUNDLE DESTINATION bin COMPONENT Runtime
  RUNTIME DESTINATION bin COMPONENT Runtime)

# man pages
install(FILES qucspowercombining.1 DESTINATION share/man/man1)
