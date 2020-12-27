set(XPMS)

# toolbar images
set(PNGS att_pi.png att_tee.png att_bridge.png)

# application images
set(ICONS)

install(FILES ${XPMS} DESTINATION share/qucs/bitmaps)
install(FILES ${PNGS} DESTINATION share/qucs/bitmaps)
install(FILES ${ICONS} DESTINATION share/qucs/bitmaps)
