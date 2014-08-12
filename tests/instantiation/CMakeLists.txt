find_package(Boost COMPONENTS unit_test_framework REQUIRED QUIET)

add_executable(instantiationTest
  InstantiationTest.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
)

target_link_libraries(instantiationTest
  ${Boost_LIBRARIES}
  answer
)

add_test(NAME instantiation COMMAND instantiationTest)
