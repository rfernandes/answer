#include "answer/WebModule.hh"

namespace answer{

WebModuleStore::WebModuleStore(){}

WebModuleStore& WebModuleStore::getInstance(){
	static WebModuleStore store;
	return store;
}

WebModuleStore::~WebModuleStore(){
	for(std::map< std::string, WebModule* >::iterator it = _store.begin(); it != _store.end();++it){
		delete it->second;
	}
}

const std::map<std::string, WebModule*>& WebModuleStore::getStore() const{
	return _store;
}

void WebModuleStore::registerModule(const std::string& name, WebModule *module){
	if (_store.count(name)){
		throw std::runtime_error("A module with the name ["+name+"] is already registered");
	}
	_store[name] = module;
}

void WebModuleStore::removeModule(const std::string& name){
	std::map< std::string, WebModule* >::iterator it = _store.find(name);
	if (it != _store.end()){
		delete it->second;
		_store.erase(it);
	}
}

}