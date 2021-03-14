# this file is part of Qucs.

# (C) 2014,2021 Guilherme Brondani Torri

# man pages
install(FILES qucs.1 DESTINATION share/man/man1)
install(FILES qucsdigi.1 DESTINATION share/man/man1)
install(FILES qucsdigilib.1 DESTINATION share/man/man1)
install(FILES qucsveri.1 DESTINATION share/man/man1)

# TODO: move to man
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucs.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucs.1)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigi.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigi.1)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigilib.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsdigilib.1)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/man/qucsveri.1.cmake.in
#               ${CMAKE_CURRENT_SOURCE_DIR}/man/qucsveri.1)
