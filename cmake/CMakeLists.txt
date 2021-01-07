# qucs-suite
# whats the difference between it and the qucs subfolder?
project(qucs CXX)
cmake_minimum_required(VERSION 3.18)
cmake_policy(VERSION 3.18)

# Set standard installation paths.
#include(GNUInstallDirs)

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

