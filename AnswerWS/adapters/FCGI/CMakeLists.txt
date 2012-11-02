set (FCGI_ADAPTER_SRC_FILES "src/FCGIAdapter.cpp")

set (FCGI_LIBRARIES "dl" "boost_thread" "fastcgipp")

add_executable (${PROJECT_NAME}-fcgi ${FCGI_ADAPTER_SRC_FILES})
target_link_libraries (${PROJECT_NAME}-fcgi
	${PROJECT_NAME}
	${PROJECT_NAME}-module
	${FCGI_LIBRARIES}
  boost_system
# 	${MANDATORY_LIBRARIES_LOCATION} # TODO: this is required because the services are linked against axis libs
	${MANDATORY_PACKAGES_LIBRARIES} # current: Apache -> AxisModule -> AnubisModules
	                                # newer: Apache -> Anubis -> AnubisModules
	                                #    A solution would be to split libAnubisWS into Axis and Non-Axis dependant
	                                #    for linking against the WS modules
)

install (TARGETS ${PROJECT_NAME}-fcgi DESTINATION /opt/wps/lib/cgi-bin)

# cmake_minimum_required (VERSION 2.6)
# cmake_policy(SET CMP0009 NEW) # suppress FOLLOW_SYMLINKS warning for GLOB_RECURSE
# set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")
# include(CMakeVersion)
# MACRO_CMAKE_VERSION_CHECKS()
# 
# 
# ###########################################################
# # PROJECT SPECIFIC INFO
# ###########################################################
# project (mod-wps)
# set (PROJECT_VERSION 0.2)
# set (DESCRIPTION "WPS Apache module")
# set (DESCRIPTION_SUMMARY "WPS Apache module")
# set (MANDATORY_LIBRARIES anubis-logger anubis-exceptions apreq2 wps-users) # missing lib with UserSession
# set (MANDATORY_LIBRARIES_WITHOUT_INCLUDES )
# set (OPTIONAL_LIBRARIES )
# set (OPTIONAL_LIBRARIES_WITHOUT_INCLUDES )
# set (MANDATORY_PACKAGES )
# set (BOOST_COMPONENTS program_options)
# set (POCO_COMPONENTS )
# set (MANDATORY_INCLUDE_CHECKS )
# set (OPTIONAL_INCLUDE_CHECKS )
# 
# 
# ###########################################################
# # INCLUDES, SOURCE FILES AND CMAKE MODULES
# ###########################################################
# include_directories (include /usr/include/apreq2 /usr/include/apache2 /usr/include/httpd /usr/include/apr-1.0 /usr/include/apr-1 /opt/wps/include)
# file ( GLOB_RECURSE SRC_FILES src/*.cpp )
# file ( GLOB_RECURSE headersHH . *.hh )
# file ( GLOB_RECURSE headersH . *.h )
# set (CMAKE_INSTALL_PREFIX /usr)
# 
# 
# ###########################################################
# # LIB DEPENDENCY CHECK
# ###########################################################
# include(DependencyCheck)
# MACRO_CHECK_DEPENDENCIES("/opt/wps/lib")
# 
# ###########################################################
# # DOXYGEN, CPPCHECK AND DEBUG/RELEASE FLAGS
# ###########################################################
# include(DoxygenDoc)
# MACRO_ADD_DOXYGEN_DOC()
# 
# include(CPPCheck)
# MACRO_ADD_CPPCHECK()
# 
# include(AnubisDebug)
# MACRO_ADD_ANUBISDEBUG()
# 
# ###########################################################
# # TARGET GENERATION
# ###########################################################
# add_library (mod_wps SHARED ${SRC_FILES})
# set_target_properties(mod_wps PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION} PREFIX "")
# target_link_libraries (mod_wps ${Boost_LIBRARIES} ${POCO_LIBRARIES_LOCATION} ${MANDATORY_LIBRARIES_LOCATION} ${MANDATORY_PACKAGES_LOCATION} ${OPTIONAL_LIBRARIES_LOCATION})
# 
# install (TARGETS mod_wps DESTINATION /usr/lib/apache2/modules COMPONENT library)
# install (FILES ${headersH} ${headersHH} DESTINATION include/lib${PROJECT_NAME} COMPONENT headers)
# 
# 
# #################################################################################################
# # DEBIAN PACKAGE
# #################################################################################################
# include(DEBHelper)
# 
# # Retrieve the development version number and save it in 'devel_version' #
# #MACRO_GET_DEVEL_VERSION_FOR_DEBIAN_PACKAGE (${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/)
# 
# set(PACKAGE_NAME "libapache2-${PROJECT_NAME}")
# set(PACKAGE_VERSION ${PROJECT_VERSION})
# set(PACKAGE_RELEASE 0)
# set(PACKAGE_MAINTAINER_NAME "Anubisnetworks")
# set(DEBIAN_ARCHITECTURE amd64)
# set(PACKAGE_SECTION libs)
# set(PACKAGE_MAINTAINER_EMAIL "devel@anubisnetworks.com")
# set(PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
# set(PACKAGE_DESCRIPTION ${DESCRIPTION_SUMMARY})
# set(PACKAGE_INSTALL_FILES "usr/lib")
# #set(PACKAGE_INSTALL_FILES_DEV "usr/include")
# MACRO_MAKE_DEBIAN_PACKAGE(library ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/)
# 
# 
