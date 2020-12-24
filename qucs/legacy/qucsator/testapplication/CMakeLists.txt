project(QucsatorInterface-Testapplication)
cmake_minimum_required(VERSION 3.16)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. qucsator)

add_executable(qucsatorInterfaceTestApplication main.cpp)
target_link_libraries(qucsatorInterfaceTestApplication PRIVATE qucsatorInterface)
