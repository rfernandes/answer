project(examples)

include_directories("include")

add_library(Calculator SHARED EXCLUDE_FROM_ALL
	"src/Calculator.cpp"
)
target_link_libraries (Calculator answer)

add_library(BasicService SHARED EXCLUDE_FROM_ALL
	"src/BasicService.cpp"
)
target_link_libraries (BasicService answer)

# add_library(LogModule SHARED EXCLUDE_FROM_ALL
# 	"src/LogModule.cpp"
# )
target_link_libraries (BasicService answer-module)

add_custom_target(examples DEPENDS Calculator BasicService LogModule)

# Generate WSDL and services XML based on specified header
include (CPP_WSDL)
CPP_WSDL(examples)
