# qucs/componenets library

include(../../../cmake/readMakeList.txt)

find_package(
  Qt5
  REQUIRED
  Widgets
  Script
  )

# Srcs
#readVariable(MakeList BASICS COMPONENT_SRCS_)
#set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
#readVariable(MakeList DEFAULT_COMPONENTS COMPONENT_SRCS_)
#set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
readVariable(MakeList OTHERCOMPONENTS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
readVariable(MakeList LUMPED_COMPONENTS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
readVariable(MakeList RF_COMPONENTS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
readVariable(MakeList TASK_ELEMENTS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
#readVariable(MakeList LOGIC_COMPONENTS COMPONENT_SRCS_)
#set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})

# Headers
readVariable(MakeList HEADERS_ HEADERS)

set(COMPONENTS_MOC_HDRS spicedialog.h optimizedialog.h
                        spicefile.h)

# An object library compiles source files but does not archive or link their object files into a library.
add_library(components SHARED ${HEADERS} ${COMPONENTS_SRCS}
                              ${COMPONENTS_MOC_SRCS})
target_include_directories(components
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../src
	${CMAKE_CURRENT_SOURCE_DIR}/../../include
	${qucs_app_BINARY_DIR}
	)
target_link_libraries(components Qt5::Widgets Qt5::Script)
target_link_libraries(components qucsschematic legacy)
set_property(TARGET components PROPERTY AUTOMOC ON)
set_property(TARGET components PROPERTY AUTORCC ON)
