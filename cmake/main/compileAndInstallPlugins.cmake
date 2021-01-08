# install dependencies of the qucs application if desired.
# Used for getting a debug environment which is ready to use
# The QUCS_PLUGPATH will be changed and all dependency libraries
# get copied into this path

if (${CompileDependencies})
    set(LIB_PATH_TEMP ${CMAKE_BINARY_DIR}/libraries)
    file(MAKE_DIRECTORY ${LIB_PATH_TEMP})

    add_compile_definitions(QUCS_PLUGPATH="${LIB_PATH_TEMP}")

    # copy the library into a common folder for all dependencies of qucs
    # so they can be used independently to the installed ones
	add_custom_target(liblegacyInstall ALL
		DEPENDS legacy
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/legacy/liblegacy.so ${LIB_PATH_TEMP}
		)

	add_custom_target(libqucsatorInstall ALL
		DEPENDS qucsator
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/legacy/qucsator/libqucsator.so ${LIB_PATH_TEMP}/legacy/libqucsator.so
		)

	add_custom_target(libcomponentsInstall ALL
		DEPENDS components
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/legacy/components/libcomponents.so ${LIB_PATH_TEMP}/legacy/libcomponents.so
		)

	add_custom_target(libpaintingsInstall ALL
		DEPENDS paintings
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/legacy/paintings/libpaintings.so ${LIB_PATH_TEMP}/legacy/libpaintings.so
		)

	add_custom_target(liblibfilesInstall ALL
		DEPENDS libfiles
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/legacy/libfiles/liblibfiles.so ${LIB_PATH_TEMP}/legacy/liblibfiles.so
		)

	add_custom_target(libmiscInstall ALL
		DEPENDS misc
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/plugins/misc/libmisc.so ${LIB_PATH_TEMP}/plugins/libmisc.so
		)

	add_custom_target(libdiagramsInstall ALL
		DEPENDS diagrams
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/plugins/diagrams/libdiagrams.so ${LIB_PATH_TEMP}/plugins/libdiagrams.so
		)
endif()

