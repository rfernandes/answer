#include "answer/Module.hh"
#include "answer/Environment.hh"
#include "XmlParams.hh"
#include "AxisCookieWrapper.hh"
#include "AxisEnvironmentWrapper.hh"
#include "AxisProviderStore.hh"
#include "AxisOperation.hh"

namespace answer{
namespace adapter{
namespace axis{

class AxisInFlowContext: public InFlowContext {
	AxisCookieJarWrapper _cookies;
	AxisEnvironmentWrapper _environment;
	AxisProviderStoreFacility _providerStore;
	AxisOperation _operation;
	XmlParams _params;

public:
	AxisInFlowContext ( const axutil_env_t * axis_env,
										struct axis2_msg_ctx * msg_ctx ) :
		_cookies ( axis_env,msg_ctx ),
		_environment ( axis_env,msg_ctx ),
		_providerStore ( axis_env,msg_ctx ),
		_operation ( axis_env,msg_ctx ),
		_params ( axis_env,msg_ctx ) {
	}

	virtual CookieJar& cookieJar() {
			return _cookies;
	}
	virtual Environment& environment() {
			return _environment;
	}
	virtual ProviderStore& providerStore() {
			return _providerStore;
	}
	virtual OperationInfo& operationInfo() {
			return _operation;
	}
	virtual answer::Params& params() {
			return _params;
	};
};

}
}
}