include(../../cmake/readMakeList.txt)

readVariable(MakeList SRCS SRCS)

add_library(diagrams ${SRC})
target_link_libraries(diagrams Qt5::Core Qt5::Widgets Qt5::Gui)
target_include_directories(diagrams PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
