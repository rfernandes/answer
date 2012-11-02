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
		virtual Environment& getCookies() = 0;
		virtual Environment& getEnvironment() = 0;
		virtual ProviderStore& getProviderStore() = 0;
		virtual OperationInfo& getOperation() = 0;
		virtual Params& getParams() = 0;
	};

	class OutFlowContext{
	public:
		virtual Environment& getEnvironment() = 0;
		virtual ProviderStore& getProviderStore() = 0;
		virtual OperationInfo& getOperation() = 0;
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
		std::map<std::string, WebModule*> _store;
		WebModuleStore();
	public:
		static WebModuleStore& getInstance();
		~WebModuleStore();
		//Takes ownership
		const std::map<std::string, WebModule*>& getStore() const;
		void registerModule(const std::string& name, WebModule *module);
		void removeModule(const std::string& name);
	};

	template <class T>
	class RegisterWebModule{
		std::string _name;
	public:
		RegisterWebModule(const std::string& name): _name(name){
			T *module = new T();
// 			std::cerr << "Registering"<< name << std::endl;
			WebModuleStore::getInstance().registerModule( _name, module );
		}

		~RegisterWebModule(){
// 			std::cerr << "UnRegistering"<< _name << std::endl;
			WebModuleStore::getInstance().removeModule( _name );
		}
	};

}
#endif //_MODULE_H_