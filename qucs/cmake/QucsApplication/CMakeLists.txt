# Qucs application creation

project(qucs_app)
cmake_minimum_required(VERSION 3.16)

# use top VERSION file
file(STRINGS ${PROJECT_SOURCE_DIR}/../../VERSION QUCS_VERSION)
message(STATUS "Configuring ${PROJECT_NAME} (GUI): VERSION ${QUCS_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2020")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")

# when true, all the dependencies (Qucsator, legacy and so on are also compiled)
option(CompileDependencies "Compile dependencies related to qucs (qucsator ...), default ON" ON)

# add compiler definitions/symbols
add_compile_definitions(PACKAGE_VERSION="${PROJECT_VERSION}")
add_compile_definitions(QUCS_PREFIX="${prefix}")
add_compile_definitions(QUCS_PLUGPATH="${pkglibdir}")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../QucsSchematicLibrary QucsSchematicLibrary)

if (${CompileDependencies})
	set(LIB_PATH_TEMP ${CMAKE_BINARY_DIR}/libraries)
	file(MAKE_DIRECTORY ${LIB_PATH_TEMP})

	add_compile_definitions(QUCS_PLUGPATH="${CMAKE_BINARY_DIR}/libraries")
	#message("QUCS_PLUGPATH: ${CMAKE_BINARY_DIR}/libraries")

	# copy the library into a common folder for all dependencies of qucs
	# so they can be used independently to the installed ones
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../legacy legacy)
	#set_target_properties(legacy PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${LIB_PATH_TEMP}) # this does not work
	if(EXISTS ${CMAKE_BINARY_DIR}/legacy/liblegacy.so)
		file(COPY ${CMAKE_BINARY_DIR}/legacy/liblegacy.so DESTINATION ${LIB_PATH_TEMP})
	endif()

	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../qucsator qucsator)
	#set_target_properties(libqucsator PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${LIB_PATH_TEMP})
	if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/qucsator/src/libqucsator.so)
		file(COPY ${CMAKE_CURRENT_BINARY_DIR}/qucsator/src/libqucsator.so DESTINATION ${LIB_PATH_TEMP})
	endif()

	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/components legacy/components)
	#set_target_properties(components PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${LIB_PATH_TEMP})
	set_target_properties(components PROPERTIES OUTPUT_NAME legacy-components)
	if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/legacy/components/liblegacy-components.so)
		file(COPY ${CMAKE_CURRENT_BINARY_DIR}/legacy/components/liblegacy-components.so DESTINATION ${LIB_PATH_TEMP})
	endif()

	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../legacy/paintings legacy/paintings)
	#set_target_properties(paintings PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIB_PATH_TEMP})
	if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/legacy/paintings/libpaintings.so)
		file(COPY ${CMAKE_CURRENT_BINARY_DIR}/legacy/paintings/libpaintings.so DESTINATION ${LIB_PATH_TEMP})
	endif()
endif()


## ------ WHY needed?
# If Git hash not defined, try to define it
#if(NOT GIT)
#  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../.git)
#    find_package(Git)
#    # Get the latest abbreviated commit hash of the working branch
#    execute_process(
#      COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1u
#      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#      OUTPUT_VARIABLE GIT_COMMIT_HASH)
#    set(GIT ${GIT_COMMIT_HASH})
#    message(STATUS "Found Git repository, last commit hash: ${GIT}")
#  endif()
#endif()

#if(UNIX AND NOT APPLE)
#  # string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE) set(BIN_INSTALL_DIR
#  # "bin") set(DOC_INSTALL_DIR "share/doc/${PROJECT_NAME_LOWERCASE}/")
#else()
#  # set(BIN_INSTALL_DIR ".") set(DOC_INSTALL_DIR ".")
#endif()

#
#add_definitions(-DHAVE_CONFIG_H)

## configure the header config.h
#configure_file("${PROJECT_SOURCE_DIR}/../config.h.cmake"
#               "${PROJECT_BINARY_DIR}/config.h")

#include_directories("${PROJECT_BINARY_DIR}")
#include_directories(${CMAKE_SOURCE_DIR})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ") # enable warning level
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ") # enable C++11

## flag not available in mingw 4.8.2, MSVC10
#if(NOT WIN32)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register ")
#endif()

## For some reason, on Windows QT_LIBRARIES point to static(?) .a libs which do
## not provide the needed symbols. They are found on the .dll libraries.
#if(WIN32)
#  link_directories("${QT_BINARY_DIR}")
#endif(WIN32)

#
# Set position independed code PIC
#
#if(UNIX AND NOT APPLE)
#  if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
#    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#  endif()
#endif()

find_package(
  Qt5
  REQUIRED
  Widgets
  Svg
  PrintSupport
  )

##
## configure Apple bundle information
##
#if(APPLE)
#  # set information on Info.plist file
#  set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
#  set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
#  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
#  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
#  set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
#  set(MACOSX_BUNDLE_GUI_IDENTIFIER
#      "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
#  set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
#  set(MACOSX_BUNDLE_ICON_FILE qucs.icns)

#  # set where in the bundle to put the icns file
#  set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/bitmaps/qucs.icns
#                              PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
#  # include the icns file in the target
#  set(QUCS_SRCS ${QUCS_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/bitmaps/qucs.icns)

#  # This tells cmake where to place the translations inside the bundle
#  # SET_SOURCE_FILES_PROPERTIES( ${LANG_SRCS} PROPERTIES MACOSX_PACKAGE_LOCATION
#  # Resources/lang ) include the translation files in the target
#  # SET(RESOURCES_SRCS ${RESOURCES_SRCS} ${LANG_SRCS})
#endif(APPLE)

##
## Set up RPATH for the project
##
#option(ENABLE_RPATH "Enable rpath support on Linux and Mac" ON)
#if(NOT CMAKE_INSTALL_RPATH)
#  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#endif()
#if(APPLE AND NOT CMAKE_INSTALL_NAME_DIR)
#  set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
#endif()
#if(UNIX AND ENABLE_RPATH)
#  set(CMAKE_SKIP_BUILD_RPATH FALSE)
#  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#  set(CMAKE_MACOSX_RPATH TRUE)
#endif()

##
## CMake's way of creating an executable
##
add_executable(qucs MACOSX_BUNDLE WIN32 ${CMAKE_CURRENT_SOURCE_DIR}/../../main/main.cpp)
target_link_libraries(qucs qucsschematic)
target_link_libraries(qucs Qt5::Widgets)
target_link_libraries(qucs Qt5::Svg)
# otherwise I'm getting /usr/bin/ld: QucsSchematicLibrary/libqucsschematic.so: undefined reference to `QPrinter::QPrinter(QPrinter::PrinterMode)'
# why do I need the printersupport?
target_link_libraries(qucs Qt5::PrintSupport)

# Don't like to include them all here. TODO: find cleaner method. Included in qucsschematic already. So can it be reused?
target_include_directories(qucs PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_include_directories(qucs PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

if(UNIX AND NOT APPLE)
	set(DESKTOP qucs.desktop)
	install(FILES ${DESKTOP}
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)

	install(FILES bitmaps/hicolor/16x16/apps/qucs.png
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/16x16/apps)
	install(FILES bitmaps/hicolor/22x22/apps/qucs.png
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/22x22/apps)
	install(FILES bitmaps/hicolor/32x32/apps/qucs.png
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/apps)
	install(FILES bitmaps/hicolor/48x48/apps/qucs.png
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps)
	install(FILES bitmaps/hicolor/64x64/apps/qucs.png
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/64x64/apps)
	install(FILES bitmaps/hicolor/128x128/apps/qucs.png
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/128x128/apps)
	install(FILES bitmaps/hicolor/256x256/apps/qucs.png
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/256x256/apps)
	install(FILES bitmaps/hicolor/512x512/apps/qucs.png
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/512x512/apps)
	install(FILES bitmaps/hicolor/scalable/apps/qucs.svg
		DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)
endif()



##
## Prepare the installation
##
#set(plugin_dest_dir bin)
#set(qtconf_dest_dir bin)
#set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}")
#if(APPLE)
#  set(plugin_dest_dir ${PROJECT_NAME}.app/Contents/MacOS)
#  set(qtconf_dest_dir ${PROJECT_NAME}.app/Contents/Resources)
#  set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.app")
#endif(APPLE)

#if(WIN32)
#  set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe")
#endif(WIN32)

##
## Install the Qucs application, on Apple, the bundle is installed as on other
## platforms it'll go into the bin directory.
##
#install(
#  TARGETS qucs
#  BUNDLE DESTINATION bin COMPONENT Runtime
#  RUNTIME DESTINATION bin COMPONENT Runtime)

## set Windows runtime location for libqucschematic See:
## http://www.cmake.org/pipermail/cmake/2010-June/037461.html
#install(
#  TARGETS qucsschematic
#  RUNTIME DESTINATION bin COMPONENT runtime
#  ARCHIVE DESTINATION lib COMPONENT devel
#  LIBRARY DESTINATION lib COMPONENT library)

##
## Install needed Qt plugins by copying directories from the qt installation One
## can cull what gets copied by using 'REGEX "..." EXCLUDE'
##
#if(APPLE)
#  install(
#    DIRECTORY "${QT_PLUGINS_DIR}/imageformats"
#    DESTINATION bin/${plugin_dest_dir}/plugins
#    COMPONENT Runtime)
#endif()
##
## install a qt.conf file this inserts some cmake code into the install script to
## write the file
##
#if(APPLE)
#  install(CODE "
#    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/bin/${qtconf_dest_dir}/qt.conf\" \"\")
#    " COMPONENT Runtime)
#endif()

## ------------------------------------------------------------------------------
## --
## Use BundleUtilities to get all other dependencies for the application to work.
## It takes a bundle or executable along with possible plugins and inspects it
## for dependencies.  If they are not system dependencies, they are copied.

## directories to look for dependencies
#if(APPLE)
#  set(DIRS ${QT_LIBRARY_DIRS})
#endif()

## Now the work of copying dependencies into the bundle/package The quotes are
## escaped and variables to use at install time have their $ escaped An
## alternative is the do a configure_file() on a script and use install(SCRIPT
## ...). Note that the image plugins depend on QtSvg and QtXml, and it got those
## copied over.
#if(APPLE)
#  install(
#    CODE "
#    file(GLOB_RECURSE QTPLUGINS
#      \"\${CMAKE_INSTALL_PREFIX}/bin/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
#    set(BU_CHMOD_BUNDLE_ITEMS ON)
#    include(BundleUtilities)
#    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
#    "
#    COMPONENT Runtime)
#endif()

## Install wrapper scripts
#if(WIN32)
#  set(SCRIPTS qucsdigi.bat qucsveri.bat qucsdigilib.bat)
#else()
#  set(SCRIPTS qucsdigi qucsveri qucsdigilib)
#endif()
#install(FILES ${SCRIPTS} DESTINATION bin/)

## To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac
## OS X where CPackConfig.cmake is created by including CPack And then there's
## ways to customize this as well set(CPACK_BINARY_DRAGNDROP ON) include(CPack)
