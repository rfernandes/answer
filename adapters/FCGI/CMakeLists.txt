set (FCGI_LIBRARIES "dl")

# set (FastCGI++_ROOT "/home/c/Code/fastcgipp-unofficial/build")

find_package(FastCGI++ 3.0 REQUIRED CONFIG)
find_package(Boost COMPONENTS system thread filesystem REQUIRED QUIET)

include_directories(
  include
)

add_executable (${PROJECT_NAME}.fcgi
  "src/FCGIAdapter.cpp"
  "src/FCGIContext.cpp"
  "src/FCGIOperationInfo.cpp"
)

target_link_libraries (${PROJECT_NAME}.fcgi
  ${PROJECT_NAME}
  ${PROJECT_NAME}-module
  fastcgipp
  ${FCGI_LIBRARIES}
  ${Boost_LIBRARIES}
)

install (TARGETS answer.fcgi DESTINATION lib COMPONENT library)