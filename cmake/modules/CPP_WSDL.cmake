MACRO(CPP_WSDL)

add_custom_command(
	TARGET ${PROJECT_NAME}
	POST_BUILD
	#OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/services.xml ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.js
	COMMAND PROJECT_NAME=${PROJECT_NAME} INPUT_PATH='${CMAKE_CURRENT_SOURCE_DIR} ${ARGN}' doxygen ${CMAKE_CURRENT_SOURCE_DIR}/../tools/doxygen_cpp_wsdl/Doxyfile 2>&1 >/dev/null
	COMMAND xsltproc --stringparam project_name ${PROJECT_NAME} --path "${CMAKE_CURRENT_BINARY_DIR}/xml" ${CMAKE_CURRENT_SOURCE_DIR}/../tools/doxygen_cpp_wsdl/services.xml.xslt namespace_web_services.xml > ${CMAKE_CURRENT_BINARY_DIR}/services.xml
	COMMAND xsltproc --path "${CMAKE_CURRENT_BINARY_DIR}/xml" ${CMAKE_CURRENT_SOURCE_DIR}/../tools/doxygen_cpp_wsdl/json_payloads.xslt namespace_web_services.xml > ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.js
	COMMAND xsltproc --stringparam project_name ${PROJECT_NAME} --path "${CMAKE_CURRENT_BINARY_DIR}/xml" ${CMAKE_CURRENT_SOURCE_DIR}/../tools/doxygen_cpp_wsdl/documentation.xslt namespace_web_services.xml > ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_documentation.html
	COMMAND xsltproc --stringparam project_name ${PROJECT_NAME} --path "${CMAKE_CURRENT_BINARY_DIR}/xml" ${CMAKE_CURRENT_SOURCE_DIR}/../tools/doxygen_cpp_wsdl/wsdl.xslt namespace_web_services.xml > ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wsdl
	COMMAND xsltproc --stringparam project_name ${PROJECT_NAME} --path "${CMAKE_CURRENT_BINARY_DIR}/xml" ${CMAKE_CURRENT_SOURCE_DIR}/../tools/doxygen_cpp_wsdl/serviceDescriptor.xslt namespace_web_services.xml > ${CMAKE_CURRENT_BINARY_DIR}/serviceDescription.xml
	COMMAND xsltproc --stringparam project_name ${PROJECT_NAME} --path "${CMAKE_CURRENT_BINARY_DIR}/xml" ${CMAKE_CURRENT_SOURCE_DIR}/../tools/doxygen_cpp_wsdl/services_js.xslt namespace_web_services.xml > ${CMAKE_CURRENT_BINARY_DIR}/services.js
	# Not yet implemented
	#COMMAND xsltproc --path "${CMAKE_CURRENT_SOURCE_DIR}/xml" ${CMAKE_CURRENT_SOURCE_DIR}/../tools/doxygen_cpp_wsdl/wsdl.xslt ${CMAKE_CURRENT_SOURCE_DIR}/xml/namespace_web_services.xml > ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.js
)

set(CPP_WSDL_OUTPUT
	${CMAKE_CURRENT_BINARY_DIR}/services.xml
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.js
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wsdl
	${CMAKE_CURRENT_BINARY_DIR}/services.js
	${CMAKE_CURRENT_BINARY_DIR}/serviceDescription.xml
)

ENDMACRO(CPP_WSDL)
