project(qucsfilter-v2 CXX C)
cmake_minimum_required(VERSION 3.16)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/readMakeList.txt)

set(CMAKE_BUILD_TYPE Debug)

# The incomplete filter-v2 only builds withs -stdlib=libstc++ OSX defaults to
# libstc++. \fixme
if(APPLE)
  set(CMAKE_CXX_FLAGS "-stdlib=libstdc++")
endif()

add_definitions(-DHAVE_CONFIG_H)

# configure the header config.h
configure_file("${PROJECT_SOURCE_DIR}/../../config.h.cmake"
	"${PROJECT_BINARY_DIR}/config.h")

include_directories("${PROJECT_BINARY_DIR}")

find_package(Qt5 REQUIRED Gui Xml Widgets)

readVariable(MakeList SRCS QUCS-FILTER2_SRCS)

add_executable(qucs-filter-v2 ${QUCS-FILTER2_SRCS})

target_link_libraries(qucs-filter-v2 Qt5::Xml Qt5::Gui Qt5::Widgets)
target_include_directories(qucs-filter-v2
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	 #PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include
	 )
set_property(TARGET qucs-filter-v2 PROPERTY AUTOMOC ON)

install(TARGETS qucs-filter-v2 DESTINATION bin)

