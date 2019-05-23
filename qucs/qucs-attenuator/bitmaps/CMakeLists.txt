
SET(XPMS
)

# toolbar images
SET(PNGS
att_pi.png
att_tee.png
att_bridge.png
att_reflection.png
qw_series.png
qw_series_CLC.png
qw_shunt.png
qw_shunt_CLC.png
)

# application images
SET(ICONS
)

INSTALL(FILES ${XPMS} DESTINATION share/qucs/bitmaps)
INSTALL(FILES ${PNGS} DESTINATION share/qucs/bitmaps)
INSTALL(FILES ${ICONS} DESTINATION share/qucs/bitmaps)

