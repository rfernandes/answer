set (MONGOOSE_SRC_FILES
"src/MongooseAdapter.cpp"
"src/MongooseContext.cpp"
)

set (MONGOOSE_LIBRARIES "dl" "mongoose")


find_package(Boost COMPONENTS filesystem program_options system REQUIRED QUIET)

include_directories(
  include
  ${Boost_INCLUDE_DIRS}
)

add_executable(mongoose_answer
${MONGOOSE_SRC_FILES}
)

target_link_libraries (mongoose_answer
  ${PROJECT_NAME}
  ${PROJECT_NAME}-module
  ${Boost_LIBRARIES}
  ${MONGOOSE_LIBRARIES}
)

install (TARGETS mongoose_answer DESTINATION bin)

add_subdirectory(include)