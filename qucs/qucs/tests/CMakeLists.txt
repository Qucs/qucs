
# Setup unit tests

# run moc when necessary
set(CMAKE_AUTOMOC ON)

# moc files are generated in the binary dir
# look there for includes
SET(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( ElementTests_SRCS ElementTests.cpp )
ADD_EXECUTABLE(ElementTests ${ElementTests_SRCS})
TARGET_LINK_LIBRARIES(ElementTests ${QT_LIBRARIES} schematic)
ADD_TEST(NAME ElementTests COMMAND ElementTests)


#SET( ComponentTests_SRCS ComponentTests.cpp)
#ADD_EXECUTABLE(ComponentTests ${ComponentTests_SRCS})
#TARGET_LINK_LIBRARIES(ComponentTests schematic components diagrams schematic dialogs paintings ${QT_LIBRARIES})
#ADD_TEST(NAME ComponentTests COMMAND ComponentTests)
#
#
#SET( SchematicTests_SRCS SchematicTests.cpp)
#ADD_EXECUTABLE(SchematicTests ${SchematicTests_SRCS})
#TARGET_LINK_LIBRARIES(SchematicTests schematic components diagrams schematic dialogs paintings ${QT_LIBRARIES})
#ADD_TEST(NAME SchematicTests COMMAND SchematicTests)
