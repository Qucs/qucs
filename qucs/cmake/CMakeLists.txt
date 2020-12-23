# qucs-suite
# whats the difference between it and the qucs subfolder?
project(qucs CXX)
cmake_minimum_required(VERSION 3.18)
cmake_policy(VERSION 3.18)

# Set standard installation paths.
include(GNUInstallDirs)

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

message(STATUS "${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}
    ${qucs-suite_BINARY_DIR}")

find_package(Qt5 REQUIRED Core Gui Script)

set(QT_DEBUG TRUE)

add_definitions(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS)

# For some reason they do not exist anymore
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucs.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucs.1)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigi.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigi.1)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigilib.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigilib.1)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsveri.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsveri.1)

# Tools
#configure_file(
#  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-activefilter/qucsactivefilter.1.cmake.in
#  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-activefilter/qucsactivefilter.1)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-filter/qucsfilter.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-filter/qucsfilter.1)
#configure_file(
#  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-attenuator/qucsattenuator.1.cmake.in
#  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-attenuator/qucsattenuator.1)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-lib/qucslib.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-lib/qucslib.1)
#configure_file(
#  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-powercombining/qucspowercombining.1.cmake.in
#  ${CMAKE_CURRENT_SOURCE_DIR}/qucs-powercombining/qucspowercombining.1)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-rescodes/qucsrescodes.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-rescodes/qucsrescodes.1)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs-transcalc/qucstrans.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/qucs-transcalc/qucstrans.1)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/main)

# add_subdirectory(man)
# add_subdirectory(plugins)
# add_subdirectory(legacy)
# add_subdirectory(qucs-activefilter)
# add_subdirectory(qucs-attenuator)
# # ADD_SUBDIRECTORY( qucs-doc )
# add_subdirectory(qucs-filter)
# add_subdirectory(qucs-lib)
# add_subdirectory(qucs-transcalc)
# add_subdirectory(qucs-rescodes)
# # ADD_SUBDIRECTORY( examples )
# add_subdirectory(translations)
# add_subdirectory(contrib)
# add_subdirectory(qucs-powercombining)

#
# Custom uninstall target
#
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
#               "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake" IMMEDIATE @ONLY)

#add_custom_target(uninstall-ui COMMAND ${CMAKE_COMMAND} -P
#                                    ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)
