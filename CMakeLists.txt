cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

###########################################################
# PROJECT SPECIFIC INFO
###########################################################
project (answer)
set (PROJECT_VERSION 0.2)
set (PROJECT_VERSION_SO 0)

add_definitions("-std=c++14 -pedantic -Wall")
######################	#####################################
# INCLUDES, SOURCE FILES AND CMAKE MODULES
###########################################################
find_package(Boost COMPONENTS system serialization date_time filesystem REQUIRED QUIET)

include_directories(SYSTEM
  ${Boost_INCLUDE_DIRS}
)

enable_testing()
include(CTest)

#Generic adapter files
set (ADAPTER_SRC_FILES "src/Adapter.cpp" "src/Codec.cpp" "src/archive/ws_xml_iarchive.cpp" "src/archive/ws_xml_oarchive.cpp" "src/Operation.cpp" "src/Exception.cpp" "src/Cookie.cpp" "src/Environment.cpp" "src/Context.cpp")

#Module support files
set (MODULE_SRC_FILES "src/Cookie.cpp" "src/Module.cpp" "src/Exception.cpp" "src/Environment.cpp" "src/Context.cpp")

file (GLOB headersHH
	include/answer/*.hh)
file (GLOB headersArchiveHH
	include/answer/archive/*.hpp)

###########################################################
# TARGET GENERATION
###########################################################
#Module support
add_library (answer-module SHARED ${MODULE_SRC_FILES})
set_target_properties(answer-module PROPERTIES
VERSION ${PROJECT_VERSION}
SOVERSION ${PROJECT_VERSION_SO})
target_link_libraries (answer-module
	${Boost_LIBRARIES}
)

target_include_directories (answer-module
PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>  
  $<INSTALL_INTERFACE:include/>  # <prefix>/include/mylib
)

#Library
add_library (answer SHARED ${ADAPTER_SRC_FILES})
set_target_properties(answer PROPERTIES
VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (answer
  "dl"
	${Boost_LIBRARIES}
)

target_include_directories (answer
PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>  
  $<INSTALL_INTERFACE:include/>  # <prefix>/include/mylib
)

add_subdirectory(examples)

option(APACHE_ADAPTER "compile the native apache module adapter" true)
option(FCGI_ADAPTER "compile the fastcgi++ adapter" true)
option(AXIS_ADAPTER "compile the axis2/C adapter" true)
option(MONGOOSE_ADAPTER "compile the Mongoose adapter" false)

if (APACHE_ADAPTER)
  add_subdirectory(adapters/ApacheModule)
endif()

if (FCGI_ADAPTER)
	add_subdirectory(adapters/FCGI)
endif()

if (AXIS_ADAPTER)
	add_subdirectory(adapters/Axis2c)
endif()

if (MONGOOSE_ADAPTER)
  add_subdirectory(adapters/Mongoose)
endif()

include(GenerateExportHeader)
generate_export_header(answer)

install (TARGETS answer EXPORT AnswerTargets
LIBRARY DESTINATION lib
INCLUDES DESTINATION include
)

install (TARGETS answer-module EXPORT AnswerTargets
LIBRARY DESTINATION lib
COMPONENT library)

install (FILES ${headersHH} DESTINATION include/answer COMPONENT Devel)
install (FILES ${headersArchiveHH} DESTINATION include/answer/archive COMPONENT Devel)

option(CodeCoverage "Build support for code coverage (requires -DCMAKE_BUILD_TYPE=Debug)" OFF)

if (CodeCoverage)
	message (STATUS "Building with code coverage support")
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage")
	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
	set (CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
	set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory(tests)

# Packaging
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/Answer/AnswerConfigVersion.cmake"
  VERSION ${Upstream_VERSION}
  COMPATIBILITY SameMajorVersion
)

export(EXPORT AnswerTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/Answer/AnswerTargets.cmake"
#   NAMESPACE Answer::
)

set(INCLUDE_INSTALL_DIR "answer/include/")

set(ConfigPackageLocation lib/cmake/answer)
configure_package_config_file(cmake/AnswerConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/Answer/AnswerConfig.cmake"
  INSTALL_DESTINATION ${ConfigPackageLocation}
  PATH_VARS INCLUDE_INSTALL_DIR
)

install(EXPORT AnswerTargets
  FILE
    AnswerTargets.cmake
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/AnswerConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/Answer/AnswerConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)