# this file is part of Qucs.

# possibly (c) 2014 Guilherme Brondani Torri
#              2020 Martin Marmsoler
#              2020 Felix Salfelder


project(qucs_lib CXX)

# this is where the library source is.
set(libqucsPath ../../src)

set(libqucsPath ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

# use top VERSION file
file(STRINGS ../../VERSION QUCS_VERSION)

set(PROJECT_VERSION "${QUCS_VERSION}")

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

include(${PROJECT_SOURCE_DIR}/../readMakeList.txt)

add_subdirectory(${libqucsPath}/bitmaps src/bitmaps) #-> added as resources
add_subdirectory(${libqucsPath}/octave src/octave)
add_subdirectory(${libqucsPath}/python src/python)
#add_subdirectory(${libqucsPath}/tests src/tests)

readVariable(${libqucsPath}/MakeList CPP_SRC QUCS_SRCS)
#readVariable(${libqucsPath}/MakeList CPP_HEADERS QUCS_HDRS)
readVariable(${libqucsPath}/MakeList UI_FILES QUCS_UI_FILES)
readVariable(${libqucsPath}/MakeList DIALOG_SRC QUCS_DIALOG_SRC)
readVariable(${libqucsPath}/MakeList DIALOG_SRC2 QUCS_DIALOG_SRC2)
list(TRANSFORM QUCS_SRCS PREPEND ${libqucsPath}/)
#list(TRANSFORM QUCS_HDRS PREPEND ${libqucsPath}/)
list(TRANSFORM QUCS_UI_FILES PREPEND ${libqucsPath}/)
list(TRANSFORM QUCS_DIALOG_SRC PREPEND ${libqucsPath}/)
list(TRANSFORM QUCS_DIALOG_SRC2 PREPEND ${libqucsPath}/)

#
# Read variable to generate rules for building source files from bitmap resources
#
#readVariable(${libqucsPath}/MakeList RESOURCES QUCS_RESOURCES)
#list(TRANSFORM QUCS_RESOURCES PREPEND ${libqucsPath}/)

## qucs library
add_library(libqucs SHARED ${QUCS_SRCS} ${QUCS_RESOURCES} ${QUCS_UI_FILES} ${QUCS_DIALOG_SRC} ${QUCS_DIALOG_SRC2})
target_include_directories(libqucs PRIVATE ${PROJECT_SOURCE_DIR}/../../include)
target_include_directories(libqucs PRIVATE ${libqucsPath})
target_include_directories(libqucs PRIVATE ${qucs_BINARY_DIR}/main)
# the order of the links is important. At least the libraries without dependencies are added
# here the Qt ones (https://stackoverflow.com/questions/51994603/cmake-qt5-undefined-reference-to-qprinterqprinterqprinterprintermode)
target_link_libraries(libqucs   Qt5::Core
                                Qt5::Gui
                                Qt5::Widgets
                                Qt5::Svg
                                Qt5::PrintSupport
                                Qt5::Script)

# rename the library to let it be libqucs (not liblibqucs)
set_target_properties(libqucs PROPERTIES OUTPUT_NAME qucs)


# set Windows runtime location for libqucschematic See:
# http://www.cmake.org/pipermail/cmake/2010-June/037461.html
install(
  TARGETS libqucs
  RUNTIME DESTINATION bin COMPONENT runtime
  ARCHIVE DESTINATION lib/qucs COMPONENT devel
  LIBRARY DESTINATION lib/qucs COMPONENT library)

