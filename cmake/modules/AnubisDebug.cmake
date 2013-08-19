MACRO(MACRO_ADD_ANUBISDEBUG)

set (CMAKE_BUILD_TYPE RELEASE)
option (BUILD_DEBUG "Activate BUILD_DEBUG=ON for debug build" OFF)
if (BUILD_DEBUG)
	set (CMAKE_BUILD_TYPE DEBUG)
endif(BUILD_DEBUG)
message(STATUS "Build type is ${CMAKE_BUILD_TYPE}")

set (CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -pedantic -g -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings")
set (CMAKE_CXX_FLAGS "-mtune=native -march=native -mfpmath=sse") 

ENDMACRO(MACRO_ADD_ANUBISDEBUG)
