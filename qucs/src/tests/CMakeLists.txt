# Setup unit tests

## run moc when necessary
#set(CMAKE_AUTOMOC ON)

## moc files are generated in the binary dir look there for includes
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(ElementTests_SRCS ElementTests.cpp)
#add_executable(ElementTests ${ElementTests_SRCS})
#target_link_libraries(ElementTests qucsschematic)
#target_link_libraries(ElementTests Qt5::Core)
#target_link_libraries(ElementTests Qt5::Test)
#add_test(NAME ElementTests COMMAND ElementTests)

#set(ComponentTests_SRCS ComponentTests.cpp)
#add_executable(ComponentTests ${ComponentTests_SRCS})
#target_link_libraries(ComponentTests qucsschematic)
#target_link_libraries(ComponentTests Qt5::Core)
#target_link_libraries(ComponentTests Qt5::Test)
#add_test(NAME ComponentTests COMMAND ComponentTests)

#set(SchematicTests_SRCS SchematicTests.cpp)
#add_executable(SchematicTests ${SchematicTests_SRCS})
#target_link_libraries(SchematicTests qucsschematic)
#target_link_libraries(SchematicTests Qt5::Widgets)
#target_link_libraries(SchematicTests Qt5::Test)
#add_test(NAME SchematicTests COMMAND SchematicTests)
