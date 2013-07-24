#ifndef _AXIS_OPERATION_HH_
#define _AXIS_OPERATION_HH_

#include "answer/Environment.hh"
#include <axis2_msg_ctx.h>

namespace answer{
	namespace adapter{
		namespace axis{

			class AxisOperation: public OperationInfo {
					std::string _service_name;
					std::string _operation_name;
					std::string _url;
			public:
					AxisOperation ( const axutil_env_t * axis_env, struct axis2_msg_ctx * msg_ctx ) {
						//TODO: unprotected code: to protect pointers access
							axis2_op_ctx_t *operation_ctx = axis2_msg_ctx_get_op_ctx ( msg_ctx, axis_env );
							axis2_op_t *operation = axis2_op_ctx_get_op ( operation_ctx, axis_env );
							axutil_qname_t *op_qname = ( axutil_qname_t * ) axis2_op_get_qname ( operation, axis_env );
							_operation_name = axutil_qname_get_localpart ( op_qname, axis_env );
							_service_name = axutil_qname_get_prefix ( op_qname, axis_env );

					axis2_endpoint_ref_t* endpoint_ref = axis2_msg_ctx_get_from (msg_ctx, axis_env);
					if (endpoint_ref) {
						const axis2_char_t *url = axis2_endpoint_ref_get_address(endpoint_ref, axis_env);
						if (url) {
						_url = url;
						}
					}
					}

					virtual const std::string& getServiceName() const {
							return _service_name;
					}
					virtual const std::string& getOperationName() const {
							return _operation_name;
					}
					virtual const std::string& getURL() const {
							return _url;
					}
			};


		}
	}
}

#endif //_AXIS_OPERATION_HH_