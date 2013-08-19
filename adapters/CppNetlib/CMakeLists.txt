set (CPPNETLIB_ADAPTER_SRC_FILES "CppNetlib.cpp")

find_package(Boost COMPONENTS system filesystem program_options thread REQUIRED QUIET)

# add_executable (${PROJECT_NAME}_cppnetlib ${CPPNETLIB_ADAPTER_SRC_FILES})
add_executable (answer_cppnetlib ${CPPNETLIB_ADAPTER_SRC_FILES})

include_directories(
	include
	/usr/include/cpp-netlib/
	${Boost_INCLUDE_DIRS}
)

target_link_libraries (${PROJECT_NAME}_cppnetlib
	${PROJECT_NAME}
	${PROJECT_NAME}-module
	${Boost_LIBRARIES}
	/usr/lib/libcppnetlib-uri.a
	dl
)

install (TARGETS ${PROJECT_NAME}_cppnetlib DESTINATION /usr/lib/${PROJECT_NAME}/)
