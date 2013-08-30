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

class AxisOutFlowContext: public OutFlowContext {
    AxisEnvironmentWrapper _environment;
    AxisProviderStoreFacility _providerStore;
    AxisOperation _operation;
public:
    AxisOutFlowContext ( const axutil_env_t * axis_env,
                         struct axis2_msg_ctx * msg_ctx ) :
        _environment ( axis_env,msg_ctx ),
        _providerStore ( axis_env, msg_ctx ),
        _operation ( axis_env,msg_ctx ) {}

    virtual Environment& environment() {
        return _environment;
    }
    virtual ProviderStore& providerStore() {
        return _providerStore;
    }
    virtual OperationInfo& operationInfo() {
        return _operation;
    }
};

}
}
}