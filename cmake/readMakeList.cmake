# read variable from file.
# (c) 2020 Felix Salfelder
# (c) 2024 David Lanzendörfer
#     GPLv3

macro( readVariable FILENAME VARIABLE OUTVAR_NAME)
    # not sure about the scoping
    unset(FILE_BUFFER)
    unset(_RESULT)
	 file( READ ${FILENAME} FILE_BUFFER )

    # foreach does not work with newlines.
    string(REGEX REPLACE "\n" "${Esc};" BUFFER "${FILE_BUFFER}")

    foreach(LINE ${BUFFER})
       if(CONT)
           set(CONT False)
           if("${LINE}" MATCHES "\\\\${Esc}$")
               set(CONT True)
           endif()
           list( APPEND _RESULT ${LINE})
           string(REGEX REPLACE "\\\\" "" LINE ${LINE})
       endif()
       if("${LINE}" MATCHES "^${VARIABLE} =")
           string(REPLACE "${VARIABLE} =" "" _RESULT ${LINE})
           if("${LINE}" MATCHES "\\\\${Esc}$")
               set(CONT True)
           endif()
           string(REGEX REPLACE "\\\\" "" LINE ${LINE})
       endif()
    endforeach()

    # turn whitespace into ;
    separate_arguments(_RESULT)

    list(TRANSFORM _RESULT STRIP)

    # seems to be global
    set(${OUTVAR_NAME} ${_RESULT})
endmacro()
