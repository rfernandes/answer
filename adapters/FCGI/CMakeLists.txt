set (FCGI_ADAPTER_SRC_FILES "src/FCGIAdapter.cpp" "src/FCGIContext.cpp" "src/FCGIOperationInfo.cpp")

set (FCGI_LIBRARIES "dl" "fastcgipp")

find_package(Boost COMPONENTS system thread filesystem REQUIRED QUIET)

include_directories("include")

add_executable (${PROJECT_NAME}.fcgi ${FCGI_ADAPTER_SRC_FILES})
target_link_libraries (${PROJECT_NAME}.fcgi
	${PROJECT_NAME}
	${PROJECT_NAME}-module
	${FCGI_LIBRARIES}
  ${Boost_LIBRARIES}
)

install (TARGETS ${PROJECT_NAME}.fcgi DESTINATION /usr/lib/${PROJECT_NAME}/)
