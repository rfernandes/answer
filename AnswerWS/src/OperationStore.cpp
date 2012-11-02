#include "answer/OperationStore.hh"

using namespace std;

namespace answer{

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


void OperationStore::registerOperation(const string& name, Operation *webMethodHandle) {
	//This removes the Class:: part of Macro registration

	size_t pos = name.rfind("::");
	std::string filteredName(pos != name.npos ? name.substr(pos + 2) : name);
	_map[filteredName] = webMethodHandle;
}

void OperationStore::removeOperation(const string& name) {
	size_t pos = name.rfind("::");
	std::string filteredName(pos != name.npos ? name.substr(pos + 2) : name);
	
	map<string, Operation*>::iterator it = _map.find(filteredName);
	if (it != _map.end()) {
		delete it->second;
		_map.erase(it);
	}
}

OperationStore& OperationStore::getInstance() {
    static OperationStore inst;
    return inst;
}

Operation& OperationStore::getOperation(const string& name) const {
    map<string, Operation*>::const_iterator it = _map.find(name);

    if (it == _map.end()) {
        throw runtime_error("Unregistered web method requested : [" + name +']');
    }
    return *it->second;
}

}