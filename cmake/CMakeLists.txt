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

set(QT_USE_QT TRUE)
# SET( QT_USE_QTXML TRUE ) SET( QT_USE_QTSVG TRUE )
set(QT_USE_QTCORE TRUE)
set(QT_USE_QTGUI TRUE)

set(QT_USE_QTSCRIPT TRUE)

set(QT_DEBUG TRUE)

# message(${QT_USE_FILE})
# include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS)

configure_file(../man/qucs.1.cmake.in
               ../man/qucs.1)
configure_file(../man/qucsdigi.1.cmake.in
               ../man/qucsdigi.1)
configure_file(../man/qucsdigilib.1.cmake.in
               ../man/qucsdigilib.1)
configure_file(../man/qucsveri.1.cmake.in
               ../man/qucsveri.1)

# Tools
configure_file(
  ../qucs-activefilter/qucsactivefilter.1.cmake.in
  ../qucs-activefilter/qucsactivefilter.1)
configure_file(../qucs-filter/qucsfilter.1.cmake.in
               ../qucs-filter/qucsfilter.1)
configure_file(
  ../qucs-attenuator/qucsattenuator.1.cmake.in
  ../qucs-attenuator/qucsattenuator.1)
configure_file(../qucs-lib/qucslib.1.cmake.in
               ../qucs-lib/qucslib.1)
configure_file(
  ../qucs-powercombining/qucspowercombining.1.cmake.in
  ../qucs-powercombining/qucspowercombining.1)
configure_file(../qucs-rescodes/qucsrescodes.1.cmake.in
               ../qucs-rescodes/qucsrescodes.1)
configure_file(../qucs-transcalc/qucstrans.1.cmake.in
               ../qucs-transcalc/qucstrans.1)

add_subdirectory(src)
add_subdirectory(main)
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
configure_file("../cmake/uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake" IMMEDIATE @ONLY)

add_custom_target(uninstall-ui COMMAND ${CMAKE_COMMAND} -P
                                    ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)
