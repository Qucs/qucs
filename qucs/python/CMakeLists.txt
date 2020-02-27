# python scripts

set(BASICS parse_result.py parse_result_example.py rc_ac_sweep.dat
           rc_ac_sweep.net)

install(FILES ${BASICS} DESTINATION share/qucs/python/)
