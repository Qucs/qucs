include(../../cmake/readMakeList.txt)

readVariable(MakeList LEGACY_SRC SRCS)
readVariable(MakeList HDR HDR)

add_library(legacy SHARED ${SRCS} ${HDR})
target_link_libraries(legacy Qt5::Core)
target_link_libraries(legacy Qt5::Gui)
target_link_libraries(legacy Qt5::Widgets)
