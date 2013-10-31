find_package(Boost COMPONENTS unit_test_framework REQUIRED QUIET)

add_executable(serializationTest
	SerializationTest.cpp
)

include_directories(
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(serializationTest
	${Boost_LIBRARIES}
	answer
)

add_test(NAME serialization COMMAND serializationTest)
