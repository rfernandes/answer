cmake_minimum_required(VERSION 2.6)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")


###########################################################
# PROJECT SPECIFIC INFO
###########################################################
project (answer)
set (PROJECT_VERSION 0.1)
set (MANDATORY_PACKAGES libxml++-2.6)

######################	#####################################
# INCLUDES, SOURCE FILES AND CMAKE MODULES
###########################################################
include_directories (include)

#Generic adapter files
set (ADAPTER_SRC_FILES "src/ws_xml_grammar.cpp" "src/Params.cpp" "src/Utils.cpp" "src/Operation.cpp" "src/OperationStore.cpp" "src/Exception.cpp" "src/Cookie.cpp" "src/Context.cpp" "src/Serialization.cpp")

#Module support files
set (MODULE_SRC_FILES "src/ws_xml_grammar.cpp" "src/Cookie.cpp" "src/WebModule.cpp" "src/Exception.cpp")

file ( GLOB headersHH
	include/answer/*.hh)
file ( GLOB headersArchiveHH
	include/answer/archive/*.hpp)
file ( GLOB headersArchiveImplHH
	include/answer/archive/impl/*.hpp)

#add_subdirectory(examples)
#add_subdirectory(modules/Authorization)
#add_subdirectory(modules/Authentication)
###########################################################
# LIB DEPENDENCY CHECK
###########################################################
include(DependencyCheck)
MACRO_CHECK_DEPENDENCIES(/usr/axis2c/lib)

# message(STATUS "ONE : ${MANDATORY_PACKAGES_LIBRARIES}")
###########################################################
# TARGET GENERATION
###########################################################
#Module component
add_library (${PROJECT_NAME}-module SHARED ${MODULE_SRC_FILES})
set_target_properties(${PROJECT_NAME}-module PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}-module
	boost_serialization
	${MANDATORY_PACKAGES_LIBRARIES}
)

add_library (${PROJECT_NAME} SHARED ${ADAPTER_SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}
	boost_serialization boost_date_time
)

add_subdirectory(adapters/ApacheModule)
add_subdirectory(adapters/Axis2c)
add_subdirectory(adapters/FCGI)

install (TARGETS ${PROJECT_NAME} DESTINATION lib COMPONENT library)
install (TARGETS ${PROJECT_NAME}-module DESTINATION lib COMPONENT library)

install (FILES ${headersHH} DESTINATION include/answer COMPONENT headers)
install (FILES ${headersArchiveHH} DESTINATION include/answer/archive COMPONENT headers)
install (FILES ${headersArchiveImplHH} DESTINATION include/answer/archive/impl COMPONENT headers)
