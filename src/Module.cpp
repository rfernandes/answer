#include "answer/Module.hh"
#include <apr-1/apr_poll.h>

using namespace std;

namespace answer{

ModuleStore::ModuleStore(){}

ModuleStore& ModuleStore::Instance(){
	static ModuleStore store;
	return store;
}

// const ModuleStore::StoreT& ModuleStore::getStore() const{
// 	return _store;
// }

void ModuleStore::registerModule(/*const string& name,*/ answer::Module*const module){
	_store.push_back(module);
}


Module::FlowStatus ModuleStore::inFlow(Context &context)
{
  for (auto &module: _store){
    FlowStatus status = module->inFlow( context );
    if (status != OK){
      return status;
    }
  }
  return OK;
}

Module::FlowStatus ModuleStore::outFlow(Context &context)
{
  for (auto &module: _store){
    FlowStatus status = module->outFlow( context );
    if (status != OK){
      return status;
    }
  }
  return OK;
}

Module::FlowStatus ModuleStore::outFlowFault(Context &context)
{
  for (auto &module: _store){
    FlowStatus status = module->outFlowFault( context );
    if (status != OK){
      return status;
    }
  }
  return OK;
}

}