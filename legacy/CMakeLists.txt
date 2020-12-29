project(legacy)
cmake_minimum_required(VERSION 3.16
	)
include(../cmake/readMakeList.txt)

# add compiler definitions/symbols
# TODO: try to remove somehow
add_compile_definitions(PACKAGE_VERSION="${PROJECT_VERSION}")

readVariable(MakeList LEGACY_SRC SRCS)
readVariable(MakeList HDR HDR)

find_package(
  Qt5
  REQUIRED
  Widgets
  Gui
  Core
  )

add_library(legacy SHARED ${SRCS})
# TODO: get rid of all of this dependecies
target_include_directories(legacy PRIVATE ${PROJECT_SOURCE_DIR}/../src
 ${PROJECT_SOURCE_DIR}/../include
 ${qucs_BINARY_DIR}/main # config.h
 )
target_link_libraries(legacy Qt5::Core)
target_link_libraries(legacy Qt5::Gui)
target_link_libraries(legacy Qt5::Widgets)

install(TARGETS legacy DESTINATION lib/qucs)

