find_package(Boost COMPONENTS unit_test_framework REQUIRED QUIET)

add_executable(codecTest
  CodecTest.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
)

target_link_libraries(codecTest
  ${Boost_LIBRARIES}
  answer
)

add_test(NAME codec COMMAND codecTest)
