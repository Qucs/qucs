project(Components-Testapplication)
cmake_minimum_required(VERSION 3.16)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/src libqucsschematic)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. components)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. legacy)

add_executable(qucsatorInterfaceTestApplication main.cpp)
target_link_libraries(qucsatorInterfaceTestApplication PRIVATE components)
