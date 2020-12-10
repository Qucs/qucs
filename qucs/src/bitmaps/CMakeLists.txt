include(../../../cmake/readMakeList.txt)

readVariable(MakeList XPMS XPMS_)

# toolbar images
readVariable(MakeList PNGS PNGS_)

# application images
readVariable(MakeList ICONS ICONS_)

# INSTALL(FILES ${XPMS_} DESTINATION share/qucs/bitmaps/) INSTALL(FILES ${PNGS_}
# DESTINATION share/qucs/bitmaps/) INSTALL(FILES ${ICONS_} DESTINATION
# share/qucs/bitmaps/)
