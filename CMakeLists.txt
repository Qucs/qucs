project(qucs-project CXX)
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

file(STRINGS "${qucs-project_SOURCE_DIR}/VERSION" QUCS_VERSION)
message(STATUS "Configuring Qucs Project: VERSION ${QUCS_VERSION}")

# enable testing for current directory and below
enable_testing()

add_subdirectory(qucs)
add_subdirectory(qucs-core)
