# qucs/componenets library

include(../../../cmake/readMakeList.txt)

find_package(
  Qt5
  REQUIRED
    Core
    Widgets
  )

# An object library compiles source files but does not archive or link their object files into a library.
add_library(legacylib SHARED ${CMAKE_CURRENT_SOURCE_DIR}/../libfiles.cpp)
target_include_directories(legacylib
	PUBLIC
	#${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../src # module.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../include # qt_compat.h
	)
target_link_libraries(legacylib Qt5::Core Qt5::Widgets)
set_property(TARGET legacylib PROPERTY AUTOMOC ON)
set_property(TARGET legacylib PROPERTY AUTORCC ON)
