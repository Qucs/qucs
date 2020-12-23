include(../../../cmake/readMakeList.txt)

find_package(
  Qt5
  REQUIRED
  Core # QDateTime
  Widgets # QGraphicsItem
  )

# Srcs
readVariable(MakeList RAW_SRC RAW_SRC)

# An object library compiles source files but does not archive or link their object files into a library.
add_library(qucsatorInterface SHARED ${RAW_SRC})
target_include_directories(qucsatorInterface
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../src # task_element.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../include #data.h
	)
target_link_libraries(qucsatorInterface Qt5::Core Qt5::Widgets)
#set_property(TARGET components PROPERTY AUTOMOC ON)
#set_property(TARGET components PROPERTY AUTORCC ON)
