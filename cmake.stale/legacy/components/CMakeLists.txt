# qucs/componenets library

include(../../readMakeList.txt)

set(compPath ${CMAKE_CURRENT_SOURCE_DIR}/../../../legacy/components)

# Srcs
readVariable(${compPath}/MakeList DEFAULT_COMPONENTS DEFAULT_)
readVariable(${compPath}/MakeList LUMPED_COMPONENTS LUMPED_)
readVariable(${compPath}/MakeList RF_COMPONENTS RF_)
readVariable(${compPath}/MakeList TASK_ELEMENTS TASK_)
#readVariable(${compPath}/MakeList NOTYET_SIM ?_)
#readVariable(${compPath}/MakeList LOGIC_COMPONENTS ?_)
readVariable(${compPath}/MakeList OTHERCOMPONENTS OTHER_)

list(TRANSFORM DEFAULT_ PREPEND ${compPath}/)
list(TRANSFORM LUMPED_ PREPEND ${compPath}/)
list(TRANSFORM RF_ PREPEND ${compPath}/)
list(TRANSFORM TASK_ PREPEND ${compPath}/)
list(TRANSFORM OTHER_ PREPEND ${compPath}/)


# Headers
#readVariable(${compPath}/MakeList HEADERS_ HEADERS_)
#list(TRANSFORM HEADERS_ PREPEND ${compPath}/)

readVariable(${compPath}/MakeList MOC MOC_)
list(TRANSFORM MOC_ PREPEND ${compPath}/)

#set(COMPONENTS_MOC_HDRS spicedialog.h optimizedialog.h
#                        spicefile.h)

# An object library compiles source files but does not archive or link their object files into a library.
#add_library(components OBJECT ${HEADERS_} ${COMPONENTS_SRCS}
#                              ${COMPONENTS_MOC_SRCS})


#set(COMPONENTS_MOC_HDRS componentdialog.h spicedialog.h optimizedialog.h
#                        spicefile.h)
#
#    QT5_WRAP_CPP(COMPONENTS_MOC_SRCS ${COMPONENTS_MOC_HDRS})
#
#

QT5_WRAP_CPP(COMPONENTS_MOC_SRCS ${MOC_})

add_library(components OBJECT
  ${DEFAULT_}
  ${LUMPED_}
  ${RF_}
  ${TASK_}
  ${OTHER_})
  #${COMPONENTS_MOC_SRCS})
  #${MOC_})

# why again? move to top most cmakelists?
find_package(
  Qt5
  REQUIRED
  Widgets
  Core
  Gui
  Script
  )

add_definitions(${QT_DEFINITIONS} -DQT_DEPRECATED_WARNINGS)

target_include_directories(components PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../include)
target_include_directories(components PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
target_include_directories(components PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../)

target_link_libraries(components Qt5::Widgets)
target_link_libraries(components Qt5::Core)
target_link_libraries(components Qt5::Gui)
target_link_libraries(components Qt5::Script)
