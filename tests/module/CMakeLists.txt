find_package(Boost COMPONENTS unit_test_framework REQUIRED QUIET)

add_executable(moduleTest
	ModuleTest.cpp
)

include_directories(
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(moduleTest
	${Boost_LIBRARIES}
	answer-module
)

add_test(module moduleTest)
	