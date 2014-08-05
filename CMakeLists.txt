cmake_minimum_required(VERSION 2.6)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

###########################################################
# PROJECT SPECIFIC INFO
###########################################################
project (answer)
set (PROJECT_VERSION 0.1)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -pedantic -Wall")
######################	#####################################
# INCLUDES, SOURCE FILES AND CMAKE MODULES
###########################################################
find_package(Boost COMPONENTS serialization date_time REQUIRED QUIET)

include_directories (BEFORE
  include
)

include_directories(SYSTEM
  ${Boost_INCLUDE_DIRS}
)

#Generic adapter files
set (ADAPTER_SRC_FILES "src/Codec.cpp" "src/archive/ws_xml_iarchive.cpp" "src/archive/ws_xml_oarchive.cpp" "src/Operation.cpp" "src/Exception.cpp" "src/Cookie.cpp" "src/Environment.cpp" "src/Context.cpp")

#Module support files
set (MODULE_SRC_FILES "src/Cookie.cpp" "src/Module.cpp" "src/Exception.cpp" "src/Environment.cpp" "src/Context.cpp")

file (GLOB headersHH
	include/answer/*.hh)
file (GLOB headersArchiveHH
	include/answer/archive/*.hpp)

###########################################################
# TARGET GENERATION
###########################################################
#Module support
add_library (${PROJECT_NAME}-module SHARED ${MODULE_SRC_FILES})
set_target_properties(${PROJECT_NAME}-module PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}-module
	${Boost_LIBRARIES}
)


#Library
add_library (${PROJECT_NAME} SHARED ${ADAPTER_SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}
	${Boost_LIBRARIES}
)

option(APACHE_ADAPTER "compile the native apache module adapter" true)
option(FCGI_ADAPTER "compile the fastcgi++ adapter" true)
option(AXIS_ADAPTER "compile the axis2/C adapter" true)
option(MONGOOSE_ADAPTER "compile the Mongoose adapter" false)

if (APACHE_ADAPTER)
	add_subdirectory(adapters/ApacheModule)
endif()

if (FCGI_ADAPTER)
	add_subdirectory(adapters/FCGI)
endif()

if (AXIS_ADAPTER)
	add_subdirectory(adapters/Axis2c)
endif()

if (MONGOOSE_ADAPTER)
  add_subdirectory(adapters/Mongoose)
endif()

install (TARGETS ${PROJECT_NAME} DESTINATION lib COMPONENT library)
install (TARGETS ${PROJECT_NAME}-module DESTINATION lib COMPONENT library)

install (FILES ${headersHH} DESTINATION include/answer COMPONENT headers)
install (FILES ${headersArchiveHH} DESTINATION include/answer/archive COMPONENT headers)

add_subdirectory(examples)

enable_testing()
include(CTest)

option(CodeCoverage "Build support for code coverage (requires -DCMAKE_BUILD_TYPE=Debug)" OFF)

if (CodeCoverage)
	message (STATUS "Building with code coverage support")
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage")
	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
	set (CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
	set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory(tests)