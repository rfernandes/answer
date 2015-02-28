set (APACHEMODULE_SRC_FILES
"src/ApacheModule.cpp"
"src/ApacheContext.cpp"
)

set (APACHEMODULE_LIBRARIES "aprutil-1" "apr-1" "apreq2")

include_directories (include /usr/include/apreq2 /usr/include/apache2 /usr/include/httpd /usr/include/apr-1.0 /usr/include/apr-1 /opt/wps/include)

find_package(Boost COMPONENTS filesystem REQUIRED QUIET)

add_library (mod_answer SHARED ${APACHEMODULE_SRC_FILES})
set_target_properties(mod_answer PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION} PREFIX "")
target_link_libraries (mod_answer
	${PROJECT_NAME}
	${PROJECT_NAME}-module
  ${Boost_LIBRARIES}
	${APACHEMODULE_LIBRARIES}
)

if ( DEFINED ENV{ANSWER_APACHE_MODULES_PATH} )
	set (ANSWER_APACHE_ADAPTER_INSTALL $ENV{ANSWER_APACHE_MODULES_PATH})
else()
	set (ANSWER_APACHE_ADAPTER_INSTALL "modules")
endif()

install (TARGETS mod_answer DESTINATION ${ANSWER_APACHE_ADAPTER_INSTALL} COMPONENT library)
