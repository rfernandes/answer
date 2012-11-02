include_directories("include")

add_library(Calculator "src/Calculator.cpp")
target_link_libraries (Calculator ${Boost_LIBRARIES} ${MANDATORY_LIBRARIES} ${MANDATORY_PACKAGES_LIBRARIES})

add_library(BasicService "src/BasicService.cpp")
target_link_libraries (BasicService ${Boost_LIBRARIES} ${MANDATORY_LIBRARIES} ${MANDATORY_PACKAGES_LIBRARIES})

add_library(Fortune "src/Fortune.cpp")
target_link_libraries (Fortune ${Boost_LIBRARIES} ${MANDATORY_LIBRARIES} ${MANDATORY_PACKAGES_LIBRARIES})
