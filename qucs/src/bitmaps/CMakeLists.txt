include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/readMakeList.txt)

readVariable(MakeList XPMS XPMS_)

# toolbar images
readVariable(MakeList PNGS PNGS_)

# application images
readVariable(MakeList ICONS ICONS_)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${XPMS_} DESTINATION share/qucs/bitmaps/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PNGS_} DESTINATION share/qucs/bitmaps/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${ICONS_} DESTINATION share/qucs/bitmaps/)
