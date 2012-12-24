#include "answer/WebModule.hh"

namespace answer{

WebModuleStore::WebModuleStore(){}

WebModuleStore& WebModuleStore::getInstance(){
	static WebModuleStore store;
	return store;
}

const WebModuleStore::StoreT& WebModuleStore::getStore() const{
	return _store;
}

void WebModuleStore::registerModule(/*const std::string& name,*/ answer::WebModule*const module){
	_store.push_back(module);
}

}