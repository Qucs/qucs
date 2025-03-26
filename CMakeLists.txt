cmake_minimum_required(VERSION 3.10)
project(qucs-suite CXX)

file(STRINGS "${qucs-suite_SOURCE_DIR}/VERSION" QUCS_VERSION)
message(STATUS "Configuring Qucs: VERSION ${QUCS_VERSION}")

set(QUCS_URL "https://sf.net/p/qucs")
set(QUCS_BUGREPORT "qucs-bugs@lists.sourceforge.net")

set(GIT unknown)
if(EXISTS ${CMAKE_SOURCE_DIR}/../.git)
  find_package(Git)
  # Get the latest abbreviated commit hash of the working branch
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1u
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH)
  set(GIT ${GIT_COMMIT_HASH})
  message(STATUS "Found Git repository, last commit hash: ${GIT}")
endif()

set(PROJECT_VERSION ${QUCS_VERSION})
option(BUILD_API_TESTS "Build extended API tests (compute intensive)" OFF)
if(BUILD_API_TESTS)
    message(STATUS "Building all the extended API tests")
else()
    message(STATUS "Not Building extended API tests")
endif()

message(STATUS "${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}
    ${qucs-suite_BINARY_DIR}")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_definitions(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucs.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucs.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigi.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigi.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigilib.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigilib.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsveri.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsveri.1)
configure_file(${PROJECT_SOURCE_DIR}/config.h.cmake
               ${PROJECT_BINARY_DIR}/config.h)

# Tools
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-activefilter/qucsactivefilter.1.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-activefilter/qucsactivefilter.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-filter/qucsfilter.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-filter/qucsfilter.1)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-attenuator/qucsattenuator.1.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-attenuator/qucsattenuator.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-lib/qucslib.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-lib/qucslib.1)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-powercombining/qucspowercombining.1.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-powercombining/qucspowercombining.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-rescodes/qucsrescodes.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-rescodes/qucsrescodes.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-transcalc/qucstrans.1.cmake.in
               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-transcalc/qucstrans.1)

#set(UNIT_TEST "")

add_custom_target(
    qucsator
    COMMAND pushd ${CMAKE_CURRENT_SOURCE_DIR}/qucsator && autoreconf -ivf && CXX=g++-12 ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR} && make && make install && popd
)

add_custom_command(
    TARGET qucsator
    COMMAND pushd ${CMAKE_CURRENT_SOURCE_DIR}/qucsator && autoreconf -ivf && CXX=g++-12 ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR} && make && make install && popd
)

add_subdirectory(man)
add_subdirectory(qucs)
add_subdirectory(qucs-activefilter)
add_subdirectory(qucs-attenuator)
#add_subdirectory(qucs-doc)
add_subdirectory(qucs-filter)
add_subdirectory(qucs-lib)
add_subdirectory(qucs-transcalc)
add_subdirectory(qucs-rescodes)
add_subdirectory(examples)
add_subdirectory(translations)
add_subdirectory(contrib)
add_subdirectory(qucs-powercombining)

# enable testing for current directory and below
enable_testing()
add_subdirectory(tests)

#
# Custom uninstall target
#
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake" IMMEDIATE @ONLY)

add_custom_target(uninstall-ui COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)
add_custom_target(subdirs COMMAND
echo "man qucs qucs-activefilter qucs-attenuator qucs-filter qucs-lib qucs-rescodes qucs-transcalc qucs-powercombining translations contrib examples tests")
