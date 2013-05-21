set (CPPNETLIB_ADAPTER_SRC_FILES "CppNetlib.cpp")

set (CPPNETLIB_LIBRARIES "boost_thread")

add_executable (${PROJECT_NAME}_cppnetlib ${CPPNETLIB_ADAPTER_SRC_FILES})
target_link_libraries (${PROJECT_NAME}_cppnetlib
	${PROJECT_NAME}
	${PROJECT_NAME}-module
	${CPPNETLIB_LIBRARIES}
)

install (TARGETS ${PROJECT_NAME}_cppnetlib DESTINATION /usr/lib/${PROJECT_NAME}/)
