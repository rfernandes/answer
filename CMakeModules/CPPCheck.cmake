
MACRO(MACRO_ADD_CPPCHECK)

set(CPPCHECK_INCLUDES "-I${CMAKE_CURRENT_SOURCE_DIR}/include")
find_file(CPPCHECK_EXECUTABLE NAMES cppcheck "cppcheck - code checking tool")
if (CPPCHECK_EXECUTABLE)
#	message(STATUS "cppcheck found")

	execute_process( COMMAND ${CPPCHECK_EXECUTABLE} "--version" OUTPUT_VARIABLE CPPCHECK_V140)
	string(REGEX REPLACE ".* " "" CPPCHECK_V140 ${CPPCHECK_V140})
	string(COMPARE LESS ${CPPCHECK_V140} "1.40" CPPCHECK_V140)
	if(CPPCHECK_V140)
		set(CPPCHECK_PARAMS "-v -q -a -s ${CPPCHECK_INCLUDES}")
	else(CPPCHECK_V140)
		set(CPPCHECK_PARAMS "-v -q --enable=all ${CPPCHECK_INCLUDES}")
	endif(CPPCHECK_V140)

	set(CPPCHECK_PARAMS "${CPPCHECK_PARAMS} ${SRC_FILES} | egrep -vi '^Include' | true")

	separate_arguments(CPPCHECK_PARAMS)

	IF(NOT TARGET cppcheck)
		add_custom_target (cppcheck ${CPPCHECK_EXECUTABLE} ${CPPCHECK_PARAMS} COMMENT "Checking code")
		message(STATUS "make cppcheck => [ Runs cppcheck for this project source code ]")
	endif(NOT TARGET cppcheck)
else (CPPCHECK_EXECUTABLE)
	message(WARNING " - cppcheck not found")
endif (CPPCHECK_EXECUTABLE)

ENDMACRO(MACRO_ADD_CPPCHECK)
