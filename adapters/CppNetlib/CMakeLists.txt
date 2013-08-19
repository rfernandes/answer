set (CPPNETLIB_ADAPTER_SRC_FILES "CppNetlib.cpp")

# set (CPPNETLIB_LIBRARIES "boost_thread")

find_package(Boost COMPONENTS system program_options thread REQUIRED QUIET)

add_executable (${PROJECT_NAME}_cppnetlib ${CPPNETLIB_ADAPTER_SRC_FILES})

include_directories(
	include
	/usr/include/cpp-netlib/
	${Boost_INCLUDE_DIRS}
)

target_link_libraries (${PROJECT_NAME}_cppnetlib
	${PROJECT_NAME}
	${PROJECT_NAME}-module
# 	${CPPNETLIB_LIBRARIES}
	${Boost_LIBRARIES}
)

install (TARGETS ${PROJECT_NAME}_cppnetlib DESTINATION /usr/lib/${PROJECT_NAME}/)
