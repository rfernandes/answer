
#Axis Adapter
set (AXIS_ADAPTER_FILES "src/AxisTransportInfo.cpp" "src/AxisContext.cpp" "src/AxisAdapter.cpp" "src/XmlParams.cpp" "src/AxisEnvironmentWrapper.cpp" "src/AxisCookieWrapper.cpp" "src/AxisProviderStore.cpp" "src/AxisCookieWrapper.cpp")

#Axis module adapter
set (AXIS_MODULE_SRC_FILES "src/AxisTransportInfo.cpp" "src/AxisModule.cpp" "src/XmlParams.cpp" "src/AxisProviderStore.cpp" "src/AxisEnvironmentWrapper.cpp" "src/AxisCookieWrapper.cpp")
# set (AXIS_MODULE_LIBRARIES "")

set (AXIS2C_BASE  /usr/axis2c/ )

include_directories (include  ${AXIS2C_BASE}/include/axis2-1.5.0)

add_library (${PROJECT_NAME}-axismodule SHARED ${AXIS_MODULE_SRC_FILES})
set_target_properties(${PROJECT_NAME}-axismodule PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}-axismodule
	${PROJECT_NAME}
	${PROJECT_NAME}-module
)

add_library (${PROJECT_NAME}-axis SHARED ${AXIS_ADAPTER_FILES})
set_target_properties(${PROJECT_NAME}-axis PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}-axis
	${PROJECT_NAME}
	axutil axis2_axiom axis2_engine axis2_parser axis2_http_sender axis2_http_receiver
	boost_serialization boost_date_time
	/usr/axis2c/lib/libmod_axis2.so
	${Boost_LIBRARIES}
  boost_system
)

install (FILES deploy/module.xml DESTINATION ${AXIS2C_BASE}/modules/module COMPONENT library)

install (TARGETS ${PROJECT_NAME}-axis DESTINATION lib COMPONENT library)
install (TARGETS ${PROJECT_NAME}-axismodule DESTINATION ${AXIS2C_BASE}/modules/module COMPONENT library)
