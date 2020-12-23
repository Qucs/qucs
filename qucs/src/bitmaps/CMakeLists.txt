include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/readMakeList.txt)

readVariable(MakeList XPMS XPMS_)

# toolbar images
readVariable(MakeList PNGS PNGS_)

# application images
readVariable(MakeList ICONS ICONS_)

install(FILES ${XPMS_} DESTINATION share/qucs/bitmaps/)
install(FILES ${PNGS_} DESTINATION share/qucs/bitmaps/)
install(FILES ${ICONS_} DESTINATION share/qucs/bitmaps/)
