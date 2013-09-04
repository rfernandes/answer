set (FCGI_LIBRARIES "dl")

find_package(FastCGI++ REQUIRED QUIET)
find_package(Boost COMPONENTS system thread filesystem REQUIRED QUIET)

include_directories(
  include
)

include_directories(SYSTEM
  ${FastCGI++_INCLUDE_DIRS}
)

add_executable (${PROJECT_NAME}.fcgi
	"src/FCGIAdapter.cpp"
	"src/FCGIContext.cpp"
	"src/FCGIOperationInfo.cpp"
)

target_link_libraries (${PROJECT_NAME}.fcgi
	${PROJECT_NAME}
	${PROJECT_NAME}-module
	${FCGI_LIBRARIES}
  ${Boost_LIBRARIES}
  ${FastCGI++_LIBRARIES}
)

install (TARGETS answer.fcgi DESTINATION lib COMPONENT library)