#ifndef _WEBMODULE_HH_
#define _WEBMODULE_HH_

#include <stdexcept>
#include <map>
#include <list>
#include <iostream>

#include "answer/Environment.hh"
#include "answer/Params.hh"

namespace answer{

	class InFlowContext{
	public:
// 		virtual ~InFlowContext(){};
		virtual CookieJar& cookieJar() = 0;
		virtual Environment& environment() = 0;
		virtual ProviderStore& providerStore() = 0;
		virtual OperationInfo& operationInfo() = 0;
		virtual Params& params() = 0;
	};

	class OutFlowContext{
	public:
		virtual Environment& environment() = 0;
		virtual ProviderStore& providerStore() = 0;
		virtual OperationInfo& operationInfo() = 0;
	};

	class ModuleException: public std::runtime_error{
	public:
    explicit ModuleException(const std::string& _arg):std::runtime_error(_arg){}
	};
	
	class ModuleAuthorizationException: public ModuleException{
	public:
		explicit ModuleAuthorizationException(const std::string& _arg):ModuleException(_arg){}
	};

	class ModuleAuthenticationException: public ModuleException{
	public:
		explicit ModuleAuthenticationException(const std::string& _arg):ModuleException(_arg){}
	};
	
	class WebModule{
	public:
		enum FlowStatus{OK, ERROR};
		virtual ~WebModule(){}
		virtual FlowStatus inFlow(InFlowContext &context) = 0;
		virtual FlowStatus outFlow(OutFlowContext &context) = 0;
		virtual FlowStatus outFlowFault(OutFlowContext &context) = 0;
	};

	class WebModuleStore{
	public:
		typedef std::list<WebModule *> StoreT;
	private:
		StoreT _store;
		WebModuleStore();
	public:
		
		static WebModuleStore& Instance();
		const StoreT& getStore() const;
		void registerModule(WebModule *const module);
	};

	template <class T>
	class RegisterWebModule{
		T module;
	public:
		RegisterWebModule(){
			WebModuleStore::Instance().registerModule(&module );
		}
	};

}
#endif //_MODULE_H_