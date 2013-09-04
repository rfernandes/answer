#ifndef _MODULE_HH_
#define _MODULE_HH_

#include <stdexcept>
#include <map>
#include <list>
#include <iostream>

#include "Macros.hh"
#include "Context.hh"

namespace answer{

	class ModuleException: public std::runtime_error{
	public:
    explicit ModuleException(const std::string& _arg):std::runtime_error(_arg){}
	};
	
	class Module{
	public:
		enum FlowStatus{OK, ERROR, SKIP};
		virtual ~Module(){}
		virtual FlowStatus inFlow(Context &context) = 0;
		virtual FlowStatus outFlow(Context &context) = 0;
		virtual FlowStatus outFlowFault(Context &context) = 0;
	};

  class ModuleStore: public Module{
  public:
    typedef std::list<Module *> StoreT;
  private:
    StoreT _store;
    ModuleStore();
  public:
    
    static ModuleStore& Instance();
//     const StoreT& getStore() const;
    void registerModule(Module *const module);

    virtual FlowStatus inFlow(Context &context);
    virtual FlowStatus outFlow(Context &context);
    virtual FlowStatus outFlowFault(Context &context);
  };

	template <class T>
	class RegisterModule{
		T module;
	public:
		RegisterModule(){
			ModuleStore::Instance().registerModule(&module);
		}
	};
}

#define ANSWER_REGISTER_MODULE(Module) \
  answer::RegisterModule<Module> ANSWER_MAKE_UNIQUE(_registrator_)

#endif //_MODULE_H_