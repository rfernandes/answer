find_package(Boost COMPONENTS unit_test_framework REQUIRED QUIET)

add_executable(registrationTest
	RegistrationTest.cpp
)

include_directories(
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(registrationTest
	${Boost_LIBRARIES}
	answer
)

add_test(registration registrationTest)
	