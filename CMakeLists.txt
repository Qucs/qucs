PROJECT(qucs-suite CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
cmake_policy(VERSION 2.8)


file (STRINGS "${qucs-suite_SOURCE_DIR}/VERSION" QUCS_VERSION)
message(STATUS "Configuring Qucs: VERSION ${QUCS_VERSION}")

SET(QUCS_URL "https://sf.net/p/qucs")
SET(QUCS_BUGREPORT "qucs-bugs@lists.sourceforge.net")

set(GIT unknown)
IF(EXISTS ${CMAKE_SOURCE_DIR}/../.git )
  FIND_PACKAGE(Git)
  # Get the latest abbreviated commit hash of the working branch
  execute_process(
    COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1u
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
  )
  set(GIT ${GIT_COMMIT_HASH})
  message(STATUS "Found Git repository, last commit hash: ${GIT}")
ENDIF()

message(STATUS "${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}
    ${qucs-suite_BINARY_DIR}" )

FIND_PACKAGE( Qt4 REQUIRED )

SET( QT_USE_QT TRUE )
#SET( QT_USE_QTXML TRUE )
#SET( QT_USE_QTSVG TRUE )
SET( QT_USE_QTCORE TRUE )
SET( QT_USE_QTGUI TRUE )

SET( QT_USE_QTSCRIPT TRUE )

SET( QT_DEBUG TRUE)

INCLUDE( ${QT_USE_FILE} )

ADD_DEFINITIONS(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS)




configure_file(./man/qucs.1.cmake.in ./man/qucs.1)
configure_file(./man/qucsdigi.1.cmake.in ./man/qucsdigi.1)
configure_file(./man/qucsdigilib.1.cmake.in ./man/qucsdigilib.1)
configure_file(./man/qucsveri.1.cmake.in ./man/qucsveri.1)

#Tools
configure_file(./qucs-activefilter/qucsactivefilter.1.cmake.in ./qucs-activefilter/qucsactivefilter.1)
configure_file(./qucs-filter/qucsfilter.1.cmake.in ./qucs-filter/qucsfilter.1)
configure_file(./qucs-attenuator/qucsattenuator.1.cmake.in ./qucs-attenuator/qucsattenuator.1)
configure_file(./qucs-lib/qucslib.1.cmake.in ./qucs-lib/qucslib.1)
configure_file(./qucs-powercombining/qucspowercombining.1.cmake.in ./qucs-powercombining/qucspowercombining.1)
configure_file(./qucs-rescodes/qucsrescodes.1.cmake.in ./qucs-rescodes/qucsrescodes.1)
configure_file(./qucs-transcalc/qucstrans.1.cmake.in ./qucs-transcalc/qucstrans.1)

ADD_SUBDIRECTORY( man )
ADD_SUBDIRECTORY( qucs )
ADD_SUBDIRECTORY( qucs-activefilter )
ADD_SUBDIRECTORY( qucs-attenuator )
#ADD_SUBDIRECTORY( qucs-doc )
ADD_SUBDIRECTORY( qucs-filter )
ADD_SUBDIRECTORY( qucs-lib )
ADD_SUBDIRECTORY( qucs-transcalc )
ADD_SUBDIRECTORY( qucs-rescodes )
#ADD_SUBDIRECTORY( examples )
ADD_SUBDIRECTORY( translations )
ADD_SUBDIRECTORY( contrib )
ADD_SUBDIRECTORY(qucs-powercombining)

#
# Custom uninstall target
#
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)


