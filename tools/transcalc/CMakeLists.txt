project(qucstrans CXX C)
cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)

# use top VERSION file
file(STRINGS ${PROJECT_SOURCE_DIR}/../VERSION QUCS_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${QUCS_VERSION}")

set(PROJECT_VERSION "${QUCS_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2014")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DHAVE_CONFIG_H)

# configure the header config.h
configure_file("${PROJECT_SOURCE_DIR}/../config.h.cmake"
               "${PROJECT_BINARY_DIR}/config.h")

include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") # enable warning level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ") # enable C++11

# flag not available in mingw 4.8.2, MSVC10
if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register ")
endif()

find_package(Qt4 REQUIRED)
set(QT_USE_QTGUI TRUE)

include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})

set(QUCSTRANS_SRCS helpdialog.cpp main.cpp optionsdialog.cpp qucstrans.cpp)

set(QUCSTRANS_HDRS
    c_microstrip.h
    coax.h
    coplanar.h
    microstrip.h
    rectwaveguide.h
    transline.h
    stripline.h
    units.h)

set(QUCSTRANS_MOC_HDRS helpdialog.h optionsdialog.h qucstrans.h)

qt4_wrap_cpp(QUCSTRANS_MOC_SRCS ${QUCSTRANS_MOC_HDRS})

set(LIB_SRC
    c_microstrip.cpp
    coax.cpp
    coplanar.cpp
    microstrip.cpp
    rectwaveguide.cpp
    transline.cpp
    stripline.cpp)

set(RESOURCES qucstrans_.qrc)

qt4_add_resources(RESOURCES_SRCS ${RESOURCES})

add_library(transcalc ${LIB_SRC})

if(APPLE)
  # set information on Info.plist file
  set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER
      "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
  set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
  set(MACOSX_BUNDLE_ICON_FILE qucstrans.icns)

  # set where in the bundle to put the icns file
  set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucstrans.icns
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  # include the icns file in the target
  set(QUCSTRANS_SRCS ${QUCSTRANS_SRCS}
                     ${CMAKE_CURRENT_SOURCE_DIR}/../qucs/bitmaps/qucstrans.icns)

endif(APPLE)

add_executable(
  qucstrans MACOSX_BUNDLE WIN32 ${QUCSTRANS_SRCS} ${QUCSTRANS_HDRS}
                                ${QUCSTRANS_MOC_SRCS} ${RESOURCES_SRCS})

target_link_libraries(qucstrans ${QT_LIBRARIES} transcalc)

# INSTALL(TARGETS qucstrans DESTINATION bin)

# ADD_SUBDIRECTORY( bitmaps ) -> added as resources
add_subdirectory(examples)

install(FILES qucstrans.1 DESTINATION share/man/man1)

#
# Prepare the installation
#
set(plugin_dest_dir bin)
set(qtconf_dest_dir bin)
set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}")
if(APPLE)
  set(plugin_dest_dir ${PROJECT_NAME}.app/Contents/MacOS)
  set(qtconf_dest_dir ${PROJECT_NAME}.app/Contents/Resources)
  set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app")
endif(APPLE)

if(WIN32)
  set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe")
endif(WIN32)

#
# Install the Qucs application, on Apple, the bundle is at the root of the
# install tree, and on other platforms it'll go into the bin directory.
#
install(
  TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION bin COMPONENT Runtime
  RUNTIME DESTINATION bin COMPONENT Runtime)

#
# Install needed Qt plugins by copying directories from the qt installation One
# can cull what gets copied by using 'REGEX "..." EXCLUDE'
#
if(APPLE)
  install(
    DIRECTORY "${QT_PLUGINS_DIR}/imageformats"
    DESTINATION bin/${plugin_dest_dir}/plugins
    COMPONENT Runtime)
endif()
#
# install a qt.conf file this inserts some cmake code into the install script to
# write the file
#
if(APPLE)
  install(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/bin/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)
endif()

# ------------------------------------------------------------------------------
# --
# Use BundleUtilities to get all other dependencies for the application to work.
# It takes a bundle or executable along with possible plugins and inspects it
# for dependencies.  If they are not system dependencies, they are copied.

# directories to look for dependencies
if(APPLE)
  set(DIRS ${QT_LIBRARY_DIRS})
endif()

# Now the work of copying dependencies into the bundle/package The quotes are
# escaped and variables to use at install time have their $ escaped An
# alternative is the do a configure_file() on a script and use install(SCRIPT
# ...). Note that the image plugins depend on QtSvg and QtXml, and it got those
# copied over.
if(APPLE)
  install(
    CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/bin/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    "
    COMPONENT Runtime)
endif()
