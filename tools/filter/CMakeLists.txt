project(qucsactivefilter CXX C)
cmake_minimum_required(VERSION 3.16)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/readMakeList.txt)

# use top VERSION file
file(STRINGS ${PROJECT_SOURCE_DIR}/../../VERSION QUCS_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${QUCS_VERSION}")

set(PROJECT_VERSION "${QUCS_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2014")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DHAVE_CONFIG_H)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ")

# flag not available in mingw 4.8.2, MSVC10
if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register ")
endif()

# configure the header config.h
configure_file("${PROJECT_SOURCE_DIR}/../../config.h.cmake"
               "${PROJECT_BINARY_DIR}/config.h")

include_directories("${PROJECT_BINARY_DIR}")

find_package(Qt5 REQUIRED Widgets Gui Svg)

add_definitions(${QT_DEFINITIONS})

readVariable(MakeList SRCS SRCS)
readVariable(MakeList RESOURCES RESOURCES)

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
  set(MACOSX_BUNDLE_ICON_FILE qucsactivefilter.icns)

  # set where in the bundle to put the icns file
  set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucsactivefilter.icns
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  set(SRCS
	  ${SRCS}
      ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucsactivefilter.icns)

endif(APPLE)

add_executable(
  qucsactivefilter MACOSX_BUNDLE WIN32
  ${SRCS} ${RESOURCES})

target_link_libraries(qucsactivefilter Qt5::Widgets Qt5::Svg Qt5::Gui)
target_include_directories(qucsactivefilter
     PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
     PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set_property(TARGET qucsactivefilter PROPERTY AUTOMOC ON)
set_property(TARGET qucsactivefilter PROPERTY AUTORCC ON)
install(
  TARGETS qucsactivefilter
  BUNDLE DESTINATION bin COMPONENT Runtime
  RUNTIME DESTINATION bin COMPONENT Runtime)

# man pages
install(FILES qucsactivefilter.1 DESTINATION share/man/man1)

