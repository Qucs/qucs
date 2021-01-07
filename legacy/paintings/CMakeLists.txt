# qucs/paintings library
project(paintings)
cmake_minimum_required(VERSION 3.16)

include(../../cmake/readMakeList.txt)

find_package(
  Qt5
  COMPONENTS
  Widgets
  REQUIRED
  )

readVariable(MakeList SRCS SRCS)
readVariable(MakeList EXTRA_DIST EXTRA_DIST)
qt5_wrap_cpp(EXTRA_DIST_MOCCED "id_dialog.h") # AUTOMOC should do the job. Don't understand why it is not working

add_library(paintings SHARED ${SRCS} ${EXTRA_DIST_MOCCED})

target_include_directories(paintings
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../src
	${CMAKE_CURRENT_SOURCE_DIR}/../../include
	${qucs_BINARY_DIR}/main # needed, if have_config is defined. config.h
	)
# libqucs is needed, because Element() is used
target_link_libraries(paintings Qt5::Widgets libqucs)
set_property(TARGET paintings PROPERTY AUTOMOC ON)

install(TARGETS paintings DESTINATION lib/qucs)

