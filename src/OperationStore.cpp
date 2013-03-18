#include "answer/OperationStore.hh"

using namespace std;

namespace answer{
//Provided by adapter
extern const char* currentService;

OperationStore::OperationStore() {
}

OperationStore::~OperationStore() {
    map<string, Operation*>::const_iterator it = _map.begin();
    for (;it != _map.end(); ++it) {
        delete it->second;
    }
}

std::list< std::string > OperationStore::getOperationList()
{
    std::list< std::string > ret;
    map<string, Operation*>::const_iterator it = _map.begin();
    for (;it != _map.end(); ++it) {
        ret.push_back(it->first);
    }
    return ret;
}

void OperationStore::registerOperation(const string& operationName, Operation *webMethodHandle) {
	//This removes the Class:: part of Macro registration
	size_t pos = operationName.rfind("::");
	std::string filteredName(pos != operationName.npos ? operationName.substr(pos + 2) : operationName);
	if (!answer::currentService){
		std::cerr << "Could not register ["<< operationName <<"] - Unknown service"<< std::endl;
	}else{
		_map[std::string(answer::currentService) + std::string("/") + filteredName] = webMethodHandle;
	}
}

/*
void OperationStore::removeOperation(const string& serviceName, const string& operationName) {
	size_t pos = serviceName.rfind("::");
	std::string filteredName(pos != serviceName.npos ? serviceName.substr(pos + 2) : serviceName);
	
	map<string, Operation*>::iterator it = _map.find(filteredName);
	if (it != _map.end()) {
		delete it->second;
		_map.erase(it);
	}
}*/

OperationStore& OperationStore::getInstance() {
    static OperationStore inst;
    return inst;
}

Operation& OperationStore::getOperation(const string& serviceName, const string& operationName) const {
    map<string, Operation*>::const_iterator it = _map.find(serviceName + "/" + operationName);
    if (it == _map.end()) {
        throw runtime_error("Unregistered web method requested : [" + serviceName +']');
    }
    return *it->second;
}

}