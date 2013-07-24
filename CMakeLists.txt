cmake_minimum_required(VERSION 2.6)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

###########################################################
# PROJECT SPECIFIC INFO
###########################################################
project (answer)
set (PROJECT_VERSION 0.1)

######################	#####################################
# INCLUDES, SOURCE FILES AND CMAKE MODULES
###########################################################
include_directories (include)

#Generic adapter files
set (ADAPTER_SRC_FILES "src/archive/ws_xml_iarchive.cpp" "src/archive/ws_xml_oarchive.cpp" "src/Params.cpp" "src/OperationStore.cpp" "src/Exception.cpp" "src/Cookie.cpp" "src/Context.cpp")

#Module support files
set (MODULE_SRC_FILES "src/Cookie.cpp" "src/WebModule.cpp" "src/Exception.cpp")

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
	boost_serialization
)

#Library
add_library (${PROJECT_NAME} SHARED ${ADAPTER_SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}
	boost_serialization boost_date_time
)

option(APACHE_ADAPTER "compile the native apache module adapter" true)
option(FCGI_ADAPTER "compile the fastcgi++ adapter" true)
option(AXIS_ADAPTER "compile the axis2/C adapter" true)
option(NETLIB_ADAPTER "compile the axis2/C adapter / tool" true)

# if (APACHE_ADAPTER || ! FCGI_ADAPTER)
# 	message(FATAL_ERROR "Must compile at least one adaptar, please check settings")
# endif()

if (APACHE_ADAPTER)
	add_subdirectory(adapters/ApacheModule)
endif()

if (FCGI_ADAPTER)
	add_subdirectory(adapters/FCGI)
endif()

if (AXIS_ADAPTER)
	add_subdirectory(adapters/Axis2c)
endif()

if (NETLIB_ADAPTER)
	message(STATUS "The netlib adapter is stil experimental")
	add_subdirectory(adapters/CppNetlib)
endif()


install (TARGETS ${PROJECT_NAME} DESTINATION lib COMPONENT library)
install (TARGETS ${PROJECT_NAME}-module DESTINATION lib COMPONENT library)

install (FILES ${headersHH} DESTINATION include/answer COMPONENT headers)
install (FILES ${headersArchiveHH} DESTINATION include/answer/archive COMPONENT headers)

add_subdirectory(examples)