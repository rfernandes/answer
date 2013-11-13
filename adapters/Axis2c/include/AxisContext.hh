#ifndef _AXIS_CONTEXT_HH_
#define _AXIS_CONTEXT_HH_

#include "answer/Context.hh"
#include "AxisEnvironmentWrapper.hh"
#include "AxisOperation.hh"
#include "AxisCookieWrapper.hh"
#include "AxisTransportInfo.hh"
#include <axis2_http_header.h>
#include <axis2_http_transport.h>

namespace answer{
	namespace adapter{
		namespace axis{

	class AxisContext: public Context {
// 		axis::AxisCookieJarWrapper _cookies;
// 		axis::AxisEnvironmentWrapper _environment;
		axis::AxisOperation _operation;
		axis::AxisTransport _transport;
		
		const axutil_env_t * _axis_env;
		struct axis2_msg_ctx * _msg_ctx;
	public:
		AxisContext ( const axutil_env_t * axis_env,
									struct axis2_msg_ctx * msg_ctx );

		~AxisContext();

		virtual OperationInfo& operationInfo() {
			return _operation;
		}
    virtual TransportInfo& transportInfo(){
			return _transport;
		}
	};

		}
	}
}

#endif //_AXIS_CONTEXT_HH_