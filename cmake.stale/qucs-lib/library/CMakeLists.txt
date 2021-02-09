set(COMPLIBS
    Bridges.lib
    Diodes.lib
    Ideal.lib
    JFETs.lib
    LEDs.lib
    MOSFETs.lib
    NMOSFETs.lib
    OpAmps.lib
    PMOSFETs.lib
    Regulators.lib
    Substrates.lib
    Transistors.lib
    Varistors.lib
    Z-Diodes.lib)

install(FILES ${COMPLIBS} DESTINATION share/qucs/library)
