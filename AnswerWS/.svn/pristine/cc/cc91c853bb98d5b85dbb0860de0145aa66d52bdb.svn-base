cmake_minimum_required(VERSION 2.6)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")
include(CMakeVersion)
MACRO_CMAKE_VERSION_CHECKS()

include(AnubisProject)
MACRO_ANUBIS_PROJECT("${CMAKE_CURRENT_SOURCE_DIR}/../../../"  "TRUE")
get_property(PROJECT_VERSION GLOBAL PROPERTY ANUBIS_PROJECT_VERSION)


###########################################################
# PROJECT SPECIFIC INFO
###########################################################
project (wps_module_audit)
set (PROJECT_VERSION 0.1)
set (DESCRIPTION "Anubisnetworks WPS webservices audit module")
set (DESCRIPTION_SUMMARY "Anubisnetworks WPS webservices audit module")
set (MANDATORY_LIBRARIES axutil axis2_axiom axis2_engine axis2_parser axis2_http_sender axis2_http_receiver anubis-axis2c-shared rbac wps-users anubis-webservice-module )
# set (MANDATORY_LIBRARIES_WITHOUT_INCLUDES pthread)
set (OPTIONAL_LIBRARIES )
set (OPTIONAL_LIBRARIES_WITHOUT_INCLUDES)
set (MANDATORY_PACKAGES)
set (BOOST_COMPONENTS program_options)
set (POCO_COMPONENTS )
set (MANDATORY_INCLUDE_CHECKS )
set (OPTIONAL_INCLUDE_CHECKS )

set (LIBRARY_NAME ${PROJECT_NAME})
set (AXIS2C_BASE  "/usr/axis2c/")
set (CMAKE_INSTALL_PREFIX /opt/wps/modules/)#/${PROJECT_NAME})

###########################################################
# INCLUDES, SOURCE FILES AND CMAKE MODULES
###########################################################

set (LIBANUBIS_WEBSERVICE_BASE ${CMAKE_CURRENT_SOURCE_DIR}/libanubis-webservice)

include_directories(	"./include/" 
			${LIBANUBIS_WEBSERVICE_BASE}/include 
			${AXIS2C_BASE}/include/axis2-1.5.0
)

file ( GLOB SRC_FILES src/*.cpp src/*.c)
file ( GLOB_RECURSE headersHH . *.hh )
file ( GLOB_RECURSE headersH . *.h )

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

###########################################################
# LIB DEPENDENCY CHECK
###########################################################
include(DependencyCheck)
MACRO_CHECK_DEPENDENCIES(
	${LIBANUBIS_WEBSERVICE_BASE}/build 
	${AXIS2C_BASE}/lib/ 
	/opt/wps/lib
)

###########################################################
# TARGET GENERATION
###########################################################
add_library (${LIBRARY_NAME} SHARED ${SRC_FILES})

set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (	${LIBRARY_NAME} 
	${Boost_LIBRARIES} 
	${MANDATORY_LIBRARIES_LOCATION} 
	${MANDATORY_PACKAGES_LIBRARIES} 
	${OPTIONAL_LIBRARIES_LOCATION}
	anubis-webservice-module
)

install (TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT library)
install (FILES ${headersHH} DESTINATION include/anubis/webservice/webmodule COMPONENT headers)
###########################################################
# DOXYGEN, CPPCHECK AND DEBUG/RELEASE FLAGS
###########################################################
include(DoxygenDoc)
MACRO_ADD_DOXYGEN_DOC()
 
include(CPPCheck)
MACRO_ADD_CPPCHECK()
 
include(AnubisDebug)
MACRO_ADD_ANUBISDEBUG()

#################################################################################################
# DEBIAN PACKAGE
#################################################################################################

include(ArchlinuxPkg)
include(DEBHelper)

set(PACKAGE_NAME "wps-axis2c-module-audit")
set(PACKAGE_VERSION ${ANUBIS_PROJECT_VERSION_REVISION})
set(PACKAGE_RELEASE 0)
set(PACKAGE_MAINTAINER_NAME "Anubisnetworks")
set(DEBIAN_ARCHITECTURE amd64)
set(ARCH_ARCHITECTURE x86_64)
set(PACKAGE_SECTION libs)
set(PACKAGE_MAINTAINER_EMAIL "devel@anubisnetworks.com")
set(PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
set(PACKAGE_DESCRIPTION ${DESCRIPTION_SUMMARY})
set(PACKAGE_INSTALL_FILES "opt/wps/")

MACRO_MAKE_DEBIAN_PACKAGE(library ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/)



