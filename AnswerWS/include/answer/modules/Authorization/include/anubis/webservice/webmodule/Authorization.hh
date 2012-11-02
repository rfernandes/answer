#ifndef WPSMODULEAUTHORIZATION_HH
#define WPSMODULEAUTHORIZATION_HH

#include <string>
#include <anubis/webservice/WebModule.hh>
#include <anubis/webservice/OperationStore.hh>
#include <anubis/webservice/Params.hh>

#include <rbac/Authorization.hh>
// // #include <libmemcached/memcached.hpp>

#include <boost/typeof/typeof.hpp>

#include <boost/bind.hpp>

namespace anubis{
namespace webservice{
namespace webmodule{

	class Authorization: public answer::WebModule{
	// 	User fetchUser(const std::string &sessionID);
//     memcache::Memcache _memcacheServer;
	public:
		Authorization();
		virtual ~Authorization(){}
		virtual FlowStatus inFlow(answer::InFlowContext &context);
		virtual FlowStatus outFlow(answer::OutFlowContext &context);
		virtual FlowStatus outFlowFault(answer::OutFlowContext &context);

	};

	class ParameterProvider{
	public:
		virtual ~ParameterProvider(){}
		virtual const Params& getParameter() const = 0;
	};

	class WebMethodAuthorizationSet: public RBAC::AuthorizationSet, public RBAC::UserProvider, public ParameterProvider{
		//These mutables make sense (but I still feel dirty)
		mutable const RBAC::User * _user;
		mutable const Params * _params;
	public:
		WebMethodAuthorizationSet();

		using AuthorizationSet::authorize;
		bool authorize(const RBAC::User& user, const answer::Params& params) const;
		virtual const RBAC::User& getUser() const;
		virtual const Params& getParameter() const;
	};

	class KeyValueParameterAuthorization: public RBAC::Authorization{
		const ParameterProvider& _provider;
		std::string _key;
		std::string _value;

	public:
		KeyValueParameterAuthorization(const ParameterProvider& provider, const std::string& key, const std::string& value);
		virtual bool authorize() const;
	};
	
	class PEP{
		std::map<std::string, WebMethodAuthorizationSet*> _authorizationsMap;
		std::map<std::string, WebMethodAuthorizationSet*> _authorizationsWebMethodsMap;
		PEP();
	public:
		~PEP();
		static PEP& getInstance();
		//TODO: These are a bit confusing...
		void registerWebAuthorizationSet(const std::string& name, WebMethodAuthorizationSet *webMethodCreator);
		void registerWebMethodAuthorization(const std::string& name, WebMethodAuthorizationSet *webMethodCreator);
		WebMethodAuthorizationSet* getWebAuthorizationSet(const std::string& name) const;
		WebMethodAuthorizationSet* getWebMethodAuthorization(const std::string& name) const;
	};

	//TODO: Merge this into PEP
	template <typename Operator, typename AuthorizationT>
	class RegisterOperationAuth: public RegisterOperation<Operator>{
	public:
		RegisterOperationAuth(const std::string& operationName, Operator op, RestPayload payLoad = AUTO):
		answer::RegisterOperation<Operator>(operationName, op, payLoad){
			size_t pos = operationName.rfind("::");
			std::string filteredName(pos != operationName.npos ? operationName.substr(pos + 2) : operationName);

			
			WebMethodAuthorizationSet *authorizationSet = NULL;
			if (! (authorizationSet = PEP::getInstance().getWebAuthorizationSet(typeid(AuthorizationT).name()) )){
				authorizationSet = new AuthorizationT();
				PEP::getInstance().registerWebAuthorizationSet( typeid(AuthorizationT).name() , authorizationSet);
			}
			PEP::getInstance().registerWebMethodAuthorization(filteredName, authorizationSet);
		}
	};

// The missing semicolon enforces the notion of simple replacement,
//  requiring the user use the macro as a function (which helps doxygen/cpp_wsdl xslt detection of registrations)
#define REGISTER_OPERATION_AUTHZ(ServiceOperation, Authorization) \
answer::webmodule::RegisterOperationAuth<BOOST_TYPEOF(&ServiceOperation), Authorization> MAKE_UNIQUE(_registrator_auth_)(#ServiceOperation, &ServiceOperation)

#define REGISTER_OPERATION_POST_AUTHZ(ServiceOperation, Authorization) \
answer::webmodule::RegisterOperationAuth<BOOST_TYPEOF(&ServiceOperation), Authorization> MAKE_UNIQUE(_registrator_auth_)(#ServiceOperation, &ServiceOperation, answer::FORCE_POST)


}
}
}

#endif // WPSMODULEAUTHORIZATION
