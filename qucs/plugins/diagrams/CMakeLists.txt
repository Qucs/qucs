set(SRC
    rectdiagram.cpp)
    
set(depHeaders
    "${CMAKE_CURRENT_SOURCE_DIR}/diagram.h"
    )

add_library(diagrams 
            ${SRC})
target_link_libraries(diagrams Qt5::Core)
target_link_libraries(diagrams Qt5::Widgets)
target_link_libraries(diagrams Qt5::Gui)
