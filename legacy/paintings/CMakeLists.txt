# qucs/paintings library

include(../../cmake/readMakeList.txt)

find_package(
  Qt5
  REQUIRED
  Widgets
  )

readVariable(MakeList SRCS SRCS)

add_library(paintings SHARED ${SRCS})
target_include_directories(paintings
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../src
	${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${qucs_BINARY_DIR}/main # config.h
        )
# qucsschematic is needed, because Element() is used
target_link_libraries(paintings Qt5::Widgets libqucs)
set_property(TARGET paintings PROPERTY AUTOMOC ON)

install(TARGETS paintings DESTINATION lib/qucs)

