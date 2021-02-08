#=========================================================================
# Program:   ALBA (Agile Library for Biomedical Applications)
# Module:    DateTimeMacro.cmake
# Language:  CMake
# Date:      $Date: 2021-01-01 12:00:00 $
# Version:   $Revision: 1.0.0.0 $
# Authors:   Nicola Vanella
#==========================================================================
# Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the above copyright notice for more information.
#=========================================================================*/

# Return the date (yyyy-mm-dd)
macro(DATE RESULT)
	if(WIN32)
		execute_process(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "(..)/(..)/(....).*" "\\3-\\2-\\1" ${RESULT} ${${RESULT}})
	elseif(UNIX)
		execute_process(COMMAND "date" "+%Y-%m-%d" OUTPUT_VARIABLE ${RESULT})
	else()
		message(SEND_ERROR "Unable to detect date")
		set(${RESULT} UNKNOWN)
	endif()
endmacro()

# Return the date (dd-mm-yyyy)
macro(DATE2 RESULT)
	if(WIN32)
		execute_process(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "(..)/(..)/(....).*" "\\1-\\2-\\3" ${RESULT} ${${RESULT}})
	elseif(UNIX)
		execute_process(COMMAND "date" "+%d-%m-%Y" OUTPUT_VARIABLE ${RESULT})
	else()
		message(SEND_ERROR "Unable to detect date")
		set(${RESULT} UNKNOWN)
	endif()
endmacro()

# Return the time (hh:mm:ss)
macro(TIME RESULT)
	if(WIN32)
		execute_process(COMMAND "cmd" " /C echo %TIME%" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "(..:..:..),(..)" "\\1" ${RESULT} ${${RESULT}})
	elseif(UNIX)
		execute_process(COMMAND "date" "+%H:%M:%S" OUTPUT_VARIABLE ${RESULT})
	else()
		message(SEND_ERROR "Unable to detect time")
		set(${RESULT} UNKNOWN)
	endif()
endmacro()