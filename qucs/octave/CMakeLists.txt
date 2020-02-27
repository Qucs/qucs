# octave scripts

set(BASICS
    getQucsVariable.m
    plotCartesian2D.m
    plotFFT2V.m
    loadQucsDataSet.m
    plotCartesian2D2V.m
    plotTimeFFT.m
    loadQucsVariable.m
    plotFFT.m
    showQucsDataSet.m)

install(FILES ${BASICS} DESTINATION share/qucs/octave/)
