#include "answer/WebModule.hh"
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
    AxisCookieWrapper _cookies;
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
        _params ( axis_env,msg_ctx ) {}

    virtual Environment& getCookies() {
        return _cookies;
    }
    virtual Environment& getEnvironment() {
        return _environment;
    }
    virtual ProviderStore& getProviderStore() {
        return _providerStore;
    }
    virtual OperationInfo& getOperation() {
        return _operation;
    }
    virtual answer::Params& getParams() {
        return _params;
    };
};

}
}
}