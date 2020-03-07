project(qucs-project CXX)
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

file(STRINGS "${qucs-project_SOURCE_DIR}/VERSION" QUCS_VERSION)
message(STATUS "Configuring Qucs Project: VERSION ${QUCS_VERSION}")

option(BUILD_DOC  "Build documentation (default ON)"   ON)

#
# Enable testing for current directory and below
#
enable_testing()

#
# Descend into sub-directories
#
add_subdirectory(qucs)
add_subdirectory(qucs-core)
if(BUILD_DOC)
add_subdirectory(qucs-doc)
endif()

#
# Run tests
#
add_test(NAME qucs_netlist_check
    COMMAND python3 run.py --qucs
    --prefix ${CMAKE_CURRENT_BINARY_DIR}/qucs/qucs/ --exclude skip.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qucs-test/)
set_property(
    TEST qucs_netlist_check
    PROPERTY
        ENVIRONMENT "QUCS_LIBDIR=${CMAKE_CURRENT_SOURCE_DIR}/qucs/qucs-lib/library")
set_property(
    TEST qucs_netlist_check
    APPEND
    PROPERTY
        ENVIRONMENT "QUCSCONV=${CMAKE_CURRENT_BINARY_DIR}/qucs-core/src/converter/qucsconv")


add_test(NAME qucsator_check
    COMMAND python3 run.py --qucsator --exclude skip.txt
    --prefix ${CMAKE_CURRENT_BINARY_DIR}/qucs-core/src/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qucs-test/)


add_test(NAME qucsator_equation_check
    COMMAND python3 run_equations.py
    --prefix ${CMAKE_CURRENT_BINARY_DIR}/qucs-core/src/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/qucs-test/)

# uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake" IMMEDIATE @ONLY)

add_custom_target(
  uninstall COMMAND ${CMAKE_COMMAND} -P
                  ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

