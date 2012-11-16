cmake_minimum_required(VERSION 2.6)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")


###########################################################
# PROJECT SPECIFIC INFO
###########################################################
project (answer)
# include(AnubisProject)

if ( NOT PROJECT_ROOT )
        set (PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif()

if (NOT LIBS_PATH )
        set (LIBS_PATH "${PROJECT_ROOT}/Extra libs")
endif()

# MACRO_ANUBIS_PROJECT(${PROJECT_ROOT}  "TRUE")
get_property(PROJECT_VERSION GLOBAL PROPERTY ANUBIS_PROJECT_VERSION)


set (DESCRIPTION "ANSWER C++ WebService framework")
set (DESCRIPTION_SUMMARY "ANSWER C++ WebService framework")
set (MANDATORY_LIBRARIES axutil axis2_axiom axis2_engine axis2_parser axis2_http_sender axis2_http_receiver anubis-axis2c-shared rbac)
set (MANDATORY_PACKAGES libxml++-2.6)
set (BOOST_COMPONENTS serialization date_time)

set (LIBRBAC_BASE ${LIBS_PATH}/librbac${LIBS_PATH_SUFFIX})
set (AXIS2C_BASE  /usr/axis2c/ )
set (CMAKE_INSTALL_PREFIX /usr/)

######################	#####################################
# INCLUDES, SOURCE FILES AND CMAKE MODULES
###########################################################
include_directories (include ${AXIS2C_BASE}/include/axis2-1.5.0 ${LIBRBAC_BASE}/include )
# file ( GLOB SRC_FILES src/*.cpp )

#Generic adapter files
set (ADAPTER_SRC_FILES "src/Params.cpp" "src/Utils.cpp" "src/Operation.cpp" "src/OperationStore.cpp" "src/Exception.cpp" "src/Cookie.cpp" "src/Context.cpp" "src/Serialization.cpp")

#Module support files
set (MODULE_SRC_FILES "src/WebModule.cpp" "src/Exception.cpp" "src/XmlParams.cpp")

file ( GLOB headersHH
	include/answer/*.hh)
file ( GLOB headersSerializationHH
	include/answer/serialization/*.hpp)

#add_subdirectory(examples)
#add_subdirectory(modules/Authorization)
#add_subdirectory(modules/Authentication)
###########################################################
# LIB DEPENDENCY CHECK
###########################################################
include(DependencyCheck)
MACRO_CHECK_DEPENDENCIES(/usr/axis2c/lib /opt/wps/lib)

# message(STATUS "ONE : ${MANDATORY_PACKAGES_LIBRARIES}")
###########################################################
# TARGET GENERATION
###########################################################
#Module component
add_library (${PROJECT_NAME}-module SHARED ${MODULE_SRC_FILES})
set_target_properties(${PROJECT_NAME}-module PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}-module
	${Boost_LIBRARIES}
	${MANDATORY_LIBRARIES}
	${MANDATORY_PACKAGES_LIBRARIES}
)

add_library (${PROJECT_NAME} SHARED ${ADAPTER_SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}
	${Boost_LIBRARIES}
)

add_subdirectory(adapters/ApacheModule)
add_subdirectory(adapters/Axis2c)
add_subdirectory(adapters/FCGI)

install (TARGETS ${PROJECT_NAME} DESTINATION lib COMPONENT library)
install (TARGETS ${PROJECT_NAME}-module DESTINATION lib COMPONENT library)

install (FILES ${headersHH} DESTINATION include/answer COMPONENT headers)
install (FILES ${headersSerializationHH} DESTINATION include/answer/serialization COMPONENT headers)


#################################################################################################
# DEBIAN PACKAGE
#################################################################################################
# include(ArchlinuxPkg)
# include(DEBHelper)
# 
# set(PACKAGE_NAME "lib${PROJECT_NAME}")
# set(PACKAGE_VERSION ${ANUBIS_PROJECT_VERSION_REVISION})
# set(PACKAGE_RELEASE 0)
# set(PACKAGE_MAINTAINER_NAME "Anubisnetworks")
# set(DEBIAN_ARCHITECTURE amd64)
# set(ARCH_ARCHITECTURE x86_64)
# set(PACKAGE_SECTION libs)
# set(PACKAGE_MAINTAINER_EMAIL "devel@anubisnetworks.com")
# set(PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
# set(PACKAGE_DESCRIPTION ${DESCRIPTION_SUMMARY})
# set(PACKAGE_INSTALL_FILES "${CMAKE_INSTALL_PREFIX}/lib/;${AXIS2C_BASE}/modules")
# set(PACKAGE_INSTALL_FILES_DEV "${CMAKE_INSTALL_PREFIX}/include/")
# set(PACKAGE_DISABLE_SHLIBS_DEPS "false")
# 
# MACRO_ADD_ARCH_PACKAGE(${PROJECT_NAME} arch)
# MACRO_MAKE_DEBIAN_PACKAGE(library ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/)
