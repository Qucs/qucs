# manuals
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucs.1.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/qucs.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucsdigi.1.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/qucsdigi.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucsdigilib.1.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/qucsdigilib.1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qucsveri.1.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/qucsveri.1)

# man pages
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qucs.1 DESTINATION share/man/man1)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qucsdigi.1 DESTINATION share/man/man1)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qucsdigilib.1 DESTINATION share/man/man1)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qucsveri.1 DESTINATION share/man/man1)
