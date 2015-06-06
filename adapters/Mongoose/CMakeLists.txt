set (MONGOOSE_SRC_FILES
"src/MongooseAdapter.cpp"
"src/MongooseContext.cpp"
)

find_package (Mongoose REQUIRED)
find_package(Boost COMPONENTS filesystem program_options system REQUIRED QUIET)

include_directories(
  include
  ${MONGOOSE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

add_executable(answer_mongoose
${MONGOOSE_SRC_FILES}
)

target_link_libraries (answer_mongoose
  ${PROJECT_NAME}
  ${PROJECT_NAME}-module
  ${Boost_LIBRARIES}
  ${MONGOOSE_LIBRARIES}
)

install (TARGETS answer_mongoose DESTINATION bin)

add_subdirectory(tests)