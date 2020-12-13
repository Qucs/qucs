# To have CMake pick a Qt installation of your choice that won't be found
# automatically, set the CMAKE_PREFIX_PATH environment variable. For example:
# "export CMAKE_PREFIX_PATH=/usr/local/trolltech/qt4.3.5"
#
# You can change the install location by running cmake like this: mkdir build;
# cd build cmake .. -DCMAKE_INSTALL_PREFIX=/new/install/prefix
#
# By default, the prefix is "/usr/local"
#
# Tests are enabled by default. To run the tests, try one of these: make test
# env CTEST_OUTPUT_ON_FAILURE=1 make test    # for verbose output

project(qucs_lib CXX)
cmake_minimum_required(VERSION 3.16)

set(coreSchematicPath ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

# use top VERSION file
file(STRINGS ${coreSchematicPath}/VERSION QUCS_VERSION)

set(PROJECT_VERSION "${QUCS_VERSION}")

# add compiler definitions/symbols
# already in qucs_app CMakeLists.txt. Maybe renaming it?
add_compile_definitions(PACKAGE_VERSION="${PROJECT_VERSION}")

set(PROJECT_VENDOR "Qucs team. This program is licensed under the GNU GPL")
set(PROJECT_COPYRIGHT_YEAR "2020")
set(PROJECT_DOMAIN_FIRST "qucs")
set(PROJECT_DOMAIN_SECOND "org")


# Only do these if this is the main project, and not if it is included through
# add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# enable testing for current directory and below
#enable_testing()

find_package(
  Qt5
  REQUIRED
  Core
  Widgets
  Gui
  Svg
  Xml
  Script
  Test
  PrintSupport
  )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/readMakeList.txt)

add_subdirectory(${coreSchematicPath}/bitmaps src/bitmaps) #-> added as resources
add_subdirectory(${coreSchematicPath}/octave src/octave)
add_subdirectory(${coreSchematicPath}/python src/python)
#add_subdirectory(${coreSchematicPath}/tests src/tests)

readVariable(${coreSchematicPath}/MakeList CPP_SRC QUCS_SRCS)
readVariable(${coreSchematicPath}/MakeList CPP_HEADERS QUCS_HDRS)
readVariable(${coreSchematicPath}/MakeList UI_FILES QUCS_UI_FILES)
readVariable(${coreSchematicPath}/MakeList DIALOG_SRC QUCS_DIALOG_SRC)
list(TRANSFORM QUCS_SRCS PREPEND ${coreSchematicPath}/)
list(TRANSFORM QUCS_HDRS PREPEND ${coreSchematicPath}/)
list(TRANSFORM QUCS_UI_FILES PREPEND ${coreSchematicPath}/)
list(TRANSFORM QUCS_DIALOG_SRC PREPEND ${coreSchematicPath}/)

#
# Read variable to generate rules for building source files from bitmap resources
#
readVariable(${coreSchematicPath}/MakeList RESOURCES QUCS_RESOURCES)
list(TRANSFORM QUCS_RESOURCES PREPEND ${coreSchematicPath}/)

## schematic library
add_library(coreSchematic OBJECT ${QUCS_SRCS} ${QUCS_RESOURCES} ${QUCS_UI_FILES} ${QUCS_DIALOG_SRC})
target_include_directories(coreSchematic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
target_include_directories(coreSchematic PRIVATE ${coreSchematicPath})
# the order of the links is important. At least the libraries without dependencies are added
# here the Qt ones (https://stackoverflow.com/questions/51994603/cmake-qt5-undefined-reference-to-qprinterqprinterqprinterprintermode)
target_link_libraries(coreSchematic Qt5::Core)
target_link_libraries(coreSchematic Qt5::Gui)
target_link_libraries(coreSchematic Qt5::Widgets)
target_link_libraries(coreSchematic Qt5::Svg)
target_link_libraries(coreSchematic Qt5::PrintSupport)
target_link_libraries(coreSchematic Qt5::Script)

## create shared library
add_library(
  qucsschematic SHARED
  # must be done, because coreSchematic is not a shared or static library, but only an object
  # for shared/ static libraries the target name is sufficent
  $<TARGET_OBJECTS:coreSchematic> #$<TARGET_OBJECTS:components>
  )
