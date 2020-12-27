# qucs/componenets library

include(../../../cmake/readMakeList.txt)

find_package(
  Qt5
  REQUIRED
    Core
    Widgets
  )

# An object library compiles source files but does not archive or link their object files into a library.
add_library(libfiles SHARED ${CMAKE_CURRENT_SOURCE_DIR}/libfiles.cpp)
target_include_directories(libfiles
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../src # module.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../include # qt_compat.h
	${qucs_app_BINARY_DIR} # config.h
	)
target_link_libraries(libfiles Qt5::Core Qt5::Widgets)
set_property(TARGET libfiles PROPERTY AUTOMOC ON)
set_property(TARGET libfiles PROPERTY AUTORCC ON)

install(TARGETS libfiles DESTINATION lib/qucs)
