include(../../cmake/readMakeList.txt)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

readVariable(MakeList SRCS SRCS)
readVariable(MakeList NOTYET NOTYET)

add_library(diagrams ${SRC} ${NOTYET})
target_link_libraries(diagrams Qt5::Widgets)
target_include_directories(diagrams
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include # element.h
	${CMAKE_CURRENT_SOURCE_DIR}/../../legacy # obsolete_paintings.h
	)
