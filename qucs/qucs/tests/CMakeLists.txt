
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

