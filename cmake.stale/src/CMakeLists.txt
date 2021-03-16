# This file is part of Qucs.
# Create core libraries

# (C) 2014-2021 Guilherme Brondani Torri
#     2020 Martin Marmsoler
#     2020 Felix Salfelder

# this is where the library source is.
set(coreSchematicPath ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

include(${CMAKE_SOURCE_DIR}/readMakeList.txt)

#add_subdirectory(${coreSchematicPath}/bitmaps src/bitmaps) #-> added as resources
#add_subdirectory(${coreSchematicPath}/octave src/octave)
#add_subdirectory(${coreSchematicPath}/python src/python)
#add_subdirectory(${coreSchematicPath}/tests src/tests)

readVariable(${coreSchematicPath}/MakeList CPP_SRC QUCS_SRCS)
#readVariable(${coreSchematicPath}/MakeList CPP_HEADERS QUCS_HDRS)
readVariable(${coreSchematicPath}/MakeList UI_FILES QUCS_UI_FILES)
readVariable(${coreSchematicPath}/MakeList DIALOG_SRC QUCS_DIALOG_SRC)
#readVariable(${coreSchematicPath}/MakeList DIALOG_SRC2 QUCS_DIALOG_SRC2)
list(TRANSFORM QUCS_SRCS PREPEND ${coreSchematicPath}/)
#list(TRANSFORM QUCS_HDRS PREPEND ${coreSchematicPath}/)
list(TRANSFORM QUCS_UI_FILES PREPEND ${coreSchematicPath}/)
list(TRANSFORM QUCS_DIALOG_SRC PREPEND ${coreSchematicPath}/)
#list(TRANSFORM QUCS_DIALOG_SRC2 PREPEND ${coreSchematicPath}/)

#
# Read variable to generate rules for building source files from bitmap resources
#
#readVariable(${coreSchematicPath}/MakeList RESOURCES QUCS_RESOURCES)
#list(TRANSFORM QUCS_RESOURCES PREPEND ${coreSchematicPath}/)

## schematic library
add_library(coreSchematic OBJECT ${QUCS_SRCS} ${QUCS_RESOURCES} ${QUCS_UI_FILES}
	             ${QUCS_DIALOG_SRC} ${QUCS_DIALOG_SRC2})

add_definitions(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS)

# BUG: define once, but where?
target_include_directories(coreSchematic PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../include)
target_include_directories(coreSchematic PRIVATE ${coreSchematicPath}/../include)
target_include_directories(coreSchematic PRIVATE ${coreSchematicPath}/..)
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
  #$<TARGET_OBJECTS:coreSchematic> #$<TARGET_OBJECTS:components>
  ""
  )
target_link_libraries(qucsschematic PUBLIC coreSchematic)

# set Windows runtime location for libqucschematic See:
# http://www.cmake.org/pipermail/cmake/2010-June/037461.html
install(
  TARGETS qucsschematic
  RUNTIME DESTINATION bin COMPONENT runtime
  ARCHIVE DESTINATION lib/qucs COMPONENT devel
  LIBRARY DESTINATION lib/qucs COMPONENT library)
