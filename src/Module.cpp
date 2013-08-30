#include "answer/Module.hh"

namespace answer{

ModuleStore::ModuleStore(){}

ModuleStore& ModuleStore::Instance(){
	static ModuleStore store;
	return store;
}

const ModuleStore::StoreT& ModuleStore::getStore() const{
	return _store;
}

void ModuleStore::registerModule(/*const std::string& name,*/ answer::Module*const module){
	_store.push_back(module);
}

}