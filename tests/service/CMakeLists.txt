find_package(Boost COMPONENTS unit_test_framework REQUIRED QUIET)

add_executable(serviceTest
	ServiceTest.cpp
)

include_directories(
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(serviceTest
	${Boost_LIBRARIES}
	answer
)

add_test(NAME service COMMAND serviceTest)
