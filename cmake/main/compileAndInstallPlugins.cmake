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
    #set_target_properties(legacy PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${LIB_PATH_TEMP}) # this does not work
    if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/legacy/liblegacy.so)
        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/legacy/liblegacy.so DESTINATION ${LIB_PATH_TEMP})
    endif()

	if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/legacy/qucsator/libqucsator.so)
		file(COPY ${CMAKE_CURRENT_BINARY_DIR}/legacy/qucsator/libqucsator.so DESTINATION ${LIB_PATH_TEMP}/legacy)
    endif()

    #set_target_properties(components PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${LIB_PATH_TEMP})
    if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/legacy/components/libcomponents.so)
        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/legacy/components/libcomponents.so DESTINATION ${LIB_PATH_TEMP}/legacy)
    endif()

    #set_target_properties(paintings PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIB_PATH_TEMP})
    if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/legacy/paintings/libpaintings.so)
        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/legacy/paintings/libpaintings.so DESTINATION ${LIB_PATH_TEMP}/legacy)
    endif()

    #set_target_properties(paintings PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIB_PATH_TEMP})
    if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/legacy/libfiles/liblibfiles.so)
        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/legacy/libfiles/liblibfiles.so DESTINATION ${LIB_PATH_TEMP}/legacy)
    endif()

     #set_target_properties(misc PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIB_PATH_TEMP})
     if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/plugins/misc/libmisc.so)
        file(COPY ${CMAKE_CURRENT_BINARY_DIR}/plugins/misc/libmisc.so DESTINATION ${LIB_PATH_TEMP}/plugins)
     endif()
endif()

