project(qucsrescodes CXX C)
cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)

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

# configure the header config.h
configure_file("${PROJECT_SOURCE_DIR}/../../config.h.cmake"
	"${PROJECT_BINARY_DIR}/config.h")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucsrescodes.1.cmake.in
	${CMAKE_CURRENT_SOURCE_DIR}/qucsrescodes.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") # enable warning level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ") # enable C++11

# flag not available in mingw 4.8.2, MSVC10
if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register ")
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

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
  set(MACOSX_BUNDLE_ICON_FILE qucsrescodes.icns)

  # set where in the bundle to put the icns file
  set_source_files_properties(
	  ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucsrescodes.icns
	PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
# include the icns file in the target
set(SRCS
	  ${SRCS}
	  ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucsrescodes.icns)

endif(APPLE)

qt5_wrap_cpp(EXTRA_DIST_MOCCED "mywidget.h") # why is this needed?

add_executable(
  qucsrescodes MACOSX_BUNDLE WIN32 ${SRCS}
  ${RESOURCES} ${EXTRA_DIST_MOCCED})
target_link_libraries(qucsrescodes
   Qt5::Core Qt5::Widgets)

target_include_directories(qucsrescodes
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE
  ${PROJECT_BINARY_DIR} # config.h
  ${PROJECT_SOURCE_DIR}/../../include # qt_compat.h
 )
set_property(TARGET qucsrescodes PROPERTY AUTOMOC ON)
set_property(TARGET qucsrescodes PROPERTY AUTORCC ON)

install(
  TARGETS qucsrescodes
  BUNDLE DESTINATION bin COMPONENT Runtime
  RUNTIME DESTINATION bin COMPONENT Runtime)

# man pages
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qucsrescodes.1 DESTINATION share/man/man1)
