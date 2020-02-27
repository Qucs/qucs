set(EXAMPLES hp.trc ustrip50fr4.trc ustrip_jansen.trc cstrip_kj_1.trc
             cstrip_kj_2.trc)

# installation of transmission lines
install(FILES ${EXAMPLES} DESTINATION share/qucs/tline)
