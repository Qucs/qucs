project(misc)
cmake_minimum_required(VERSION 3.16)
include(../../cmake/readMakeList.txt)

find_package(
  Qt5
  REQUIRED
  Core # QTextStream
  Widgets # QGraphicsItem
  )

# Srcs
readVariable(MakeList RAW_SRCS SRCS)

# An object library compiles source files but does not archive or link their object files into a library.
add_library(misc SHARED ${SRCS})
target_include_directories(misc
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../src # command.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../include # object.h
        ${qucs_BINARY_DIR}/main # config.h
	)
target_link_libraries(misc Qt5::Core Qt5::Widgets)
#set_property(TARGET misc PROPERTY AUTOMOC ON)
#set_property(TARGET misc PROPERTY AUTORCC ON)

install(TARGETS misc DESTINATION lib/qucs/plugins)

