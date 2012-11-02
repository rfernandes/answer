#include "anubis/webservice/webmodule/Authorization.hh"

using namespace std;
using namespace answer;

namespace anubis {
namespace webservice {
namespace webmodule{
	WebMethodAuthorizationSet::WebMethodAuthorizationSet():_user(NULL), _params(NULL){}

	bool WebMethodAuthorizationSet::authorize(const RBAC::User& user, const Params& params) const{
		bool flag = false; //DENY ALL
		_user = &user;
		_params = &params;
		flag = authorize();
		_user = NULL;
		_params = NULL;
		return flag;
	}

	const Params& WebMethodAuthorizationSet::getParameter() const
	{
		return *_params;
	}

	const RBAC::User& WebMethodAuthorizationSet::getUser() const
	{
		return *_user;
	}

	KeyValueParameterAuthorization::KeyValueParameterAuthorization(const ParameterProvider& provider, const std::string& key, const std::string& value):
		_provider(provider),
		_key(key),
		_value(value)
		{}

	bool KeyValueParameterAuthorization::authorize() const{
		return _provider.getParameter().getParam(_key) == _value;
	}

	Authorization::Authorization(){
	}

	answer::WebModule::FlowStatus Authorization::inFlow(InFlowContext &context){
		try{
			const RBAC::User &user(boost::any_cast<RBAC::User>(context.getProviderStore().getProvider("user")));
		
			
			if (!PEP::getInstance().getWebMethodAuthorization(context.getOperation().getOperationName())->authorize(user, context.getParams())){
				throw ModuleAuthorizationException("Authorization failure for operation: " + context.getOperation().getName());
			}
		}catch(std::runtime_error &ex){
			throw ModuleAuthorizationException(ex.what());
		}
		return OK;
	}

	answer::WebModule::FlowStatus Authorization::outFlow(OutFlowContext &/*context*/){
		//We currently have no outFlow handling
		return OK;
	}

	answer::WebModule::FlowStatus Authorization::outFlowFault(OutFlowContext &/*context*/){
		//We currently have no outFlow Fault handling
		return OK;
	}

	static RegisterWebModule<Authorization> _register("Authorization");

	WebMethodAuthorizationSet* PEP::getWebAuthorizationSet(const std::string& name) const{
		std::map< std::string, WebMethodAuthorizationSet* >::const_iterator it = _authorizationsMap.find(name);
		return it != _authorizationsMap.end() ? it->second: NULL;
	}

	WebMethodAuthorizationSet* PEP::getWebMethodAuthorization(const std::string& name) const{
		std::map< std::string, WebMethodAuthorizationSet* >::const_iterator it = _authorizationsWebMethodsMap.find(name);
		return it != _authorizationsWebMethodsMap.end() ? it->second: NULL;
	}
	
	void PEP::registerWebAuthorizationSet(const std::string& name, WebMethodAuthorizationSet* webMethodCreator)
	{
		if (!_authorizationsMap.count(name)){
			_authorizationsMap[name] = webMethodCreator;
		}
	}

	void PEP::registerWebMethodAuthorization(const std::string& name, WebMethodAuthorizationSet* webMethodCreator)
	{
		if (!_authorizationsWebMethodsMap.count(name)){
			_authorizationsWebMethodsMap[name] = webMethodCreator;
		}
	}

	PEP::PEP(){}
	
	PEP& PEP::getInstance()
	{
		static PEP instance;
		return instance;
	}

	PEP::~PEP(){
		std::map< string, WebMethodAuthorizationSet* >::iterator vit = _authorizationsMap.begin();
		for (;vit != _authorizationsMap.end(); ++vit) {
			delete vit->second;
		}
	}

}
}
}
