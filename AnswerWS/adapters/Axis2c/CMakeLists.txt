
#Axis Adapter
set (AXIS_ADAPTER_FILES "src/AxisAdapter.cpp" "src/AxisProviderStore.cpp")

#Axis module adapter
set (AXIS_MODULE_SRC_FILES  "src/AxisModule.cpp" "src/AxisEnvironmentWrapper.cpp" "src/AxisCookieWrapper.cpp")
# set (AXIS_MODULE_LIBRARIES "")

include_directories (include)

add_library (${PROJECT_NAME}-axismodule SHARED ${AXIS_MODULE_SRC_FILES})
set_target_properties(${PROJECT_NAME}-axismodule PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}-axismodule
	${PROJECT_NAME}-module
# 	${Boost_LIBRARIES}
# 	${MANDATORY_LIBRARIES_LOCATION}
# 	${MANDATORY_PACKAGES_LIBRARIES}
)

add_library (${PROJECT_NAME}-axis SHARED ${AXIS_ADAPTER_FILES})
set_target_properties(${PROJECT_NAME}-axis PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION})
target_link_libraries (${PROJECT_NAME}-axis
	${PROJECT_NAME}
	/usr/axis2c/lib/libmod_axis2.so
# 	${PROJECT_NAME}-module
	${Boost_LIBRARIES}
  boost_system
	${MANDATORY_LIBRARIES_LOCATION} 
	${MANDATORY_PACKAGES_LIBRARIES}
)

install (FILES deploy/module.xml DESTINATION ${AXIS2C_BASE}/modules/module COMPONENT library)

install (TARGETS ${PROJECT_NAME}-axis DESTINATION lib COMPONENT library)
install (TARGETS ${PROJECT_NAME}-axismodule DESTINATION ${AXIS2C_BASE}/modules/module COMPONENT library)
