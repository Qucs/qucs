# qucs/componenets library

include(../../../cmake/readMakeList.txt)

# Srcs
readVariable(MakeList BASICS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
readVariable(MakeList DEFAULT_COMPONENTS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
readVariable(MakeList OTHERCOMPONENTS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
readVariable(MakeList LUMPED_COMPONENTS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
readVariable(MakeList RF_COMPONENTS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
readVariable(MakeList TASK_ELEMENTS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})
readVariable(MakeList LOGIC_COMPONENTS COMPONENT_SRCS_)
set(COMPONENTS_SRCS ${COMPONENTS_SRCS} ${COMPONENT_SRCS_})

# Headers
readVariable(MakeList HEADERS_ HEADERS)

set(COMPONENTS_MOC_HDRS spicedialog.h optimizedialog.h
                        spicefile.h)

# An object library compiles source files but does not archive or link their object files into a library.
add_library(components OBJECT ${HEADERS} ${COMPONENTS_SRCS}
                              ${COMPONENTS_MOC_SRCS})
target_link_libraries(components Qt5::Widgets)
