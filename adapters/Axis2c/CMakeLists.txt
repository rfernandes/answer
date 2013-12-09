set (AXIS2C_BASE "/usr/axis2c/"
)

#Axis Adapter
set (AXIS_ADAPTER_FILES "src/AxisContext.cpp" "src/AxisAdapter.cpp")

#Axis module adapter
set (AXIS_MODULE_SRC_FILES "src/AxisModule.cpp")
set (AXIS_MODULE_LIBRARIES
"${AXIS2C_BASE}/lib/libaxutil.so"
"${AXIS2C_BASE}/lib/libaxis2_axiom.so"
"${AXIS2C_BASE}/lib/libaxis2_engine.so"
"${AXIS2C_BASE}/lib/libaxis2_parser.so"
"${AXIS2C_BASE}/lib/libaxis2_http_sender.so"
"${AXIS2C_BASE}/lib/libaxis2_http_receiver.so"
)


find_package(Boost COMPONENTS system thread filesystem REQUIRED QUIET)

include_directories(SYSTEM
${AXIS2C_BASE}/include/axis2-1.6.0
)

include_directories(
include
)


add_library (${PROJECT_NAME}-axismodule SHARED ${AXIS_MODULE_SRC_FILES})
set_target_properties(${PROJECT_NAME}-axismodule PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}-axismodule
	${PROJECT_NAME}-module
	"${AXIS2C_BASE}/lib/libmod_axis2.so"
)

add_library (${PROJECT_NAME}-axis SHARED ${AXIS_ADAPTER_FILES})
set_target_properties(${PROJECT_NAME}-axis PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}-axis
	${PROJECT_NAME}
	${AXIS_MODULE_LIBRARIES}
	${Boost_LIBRARIES}
)

install (FILES deploy/module.xml DESTINATION ${AXIS2C_BASE}/modules/module COMPONENT library)

install (TARGETS ${PROJECT_NAME}-axis DESTINATION lib COMPONENT library)
install (TARGETS ${PROJECT_NAME}-axismodule DESTINATION ${AXIS2C_BASE}/modules/module COMPONENT library)

