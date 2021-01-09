# qucs-suite
# whats the difference between it and the qucs subfolder?
project(qucs CXX)
cmake_minimum_required(VERSION 3.18)
cmake_policy(VERSION 3.18)

# Set standard installation paths.
#include(GNUInstallDirs)

option(COMPILE_TOOLS "Compile external qucs tools. Default: ON" ON)

set(QUCS_URL "https://sf.net/p/qucs")
set(QUCS_BUGREPORT "qucs-bugs@lists.sourceforge.net")

include(determineGitCommitHash.cmake)
determineGitCommitHash()

message(STATUS "${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}
    ${qucs-suite_BINARY_DIR}")

find_package(Qt5 REQUIRED Core Gui Script)

set(QT_DEBUG TRUE)

add_definitions(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS)
add_compile_definitions(HAVE_CONFIG_H)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/main)

set(QucsPath "${CMAKE_CURRENT_SOURCE_DIR}/..")
add_subdirectory(${QucsPath}/man man)
# add_subdirectory(plugins)
# add_subdirectory(legacy)

if (COMPILE_TOOLS)
	add_subdirectory(${QucsPath}/tools/activefilter tools/activefilter)
	add_subdirectory(${QucsPath}/tools/attenuator tools/attenuator)
	add_subdirectory(${QucsPath}/tools/filter tools/filter)
	add_subdirectory(${QucsPath}/tools/transcalc tools/transcalc)
	add_subdirectory(${QucsPath}/tools/rescodes tools/rescodes)
	add_subdirectory(${QucsPath}/tools/powercombining tools/powercombining)
endif()
# add_subdirectory( qucs-doc )
add_subdirectory(${QucsPath}/qucs-lib qucs-lib)
add_subdirectory(${QucsPath}/examples examples)
add_subdirectory(${QucsPath}/translations translations)
# add_subdirectory(contrib)

#
# Custom uninstall target
#
#configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
#               "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake" IMMEDIATE @ONLY)

#add_custom_target(uninstall-ui COMMAND ${CMAKE_COMMAND} -P
#                                    ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

