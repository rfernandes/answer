#include "answer/Operation.hh"

using namespace std;

namespace answer{

OperationStore::OperationStore() {}

list< string > OperationStore::operationList()
{
  list< string > ret;
  for (const auto &item: _map){
    ret.push_back(item.first);
  }
  return ret;
}

void OperationStore::registerOperation( const string& serviceName, const string& operationName, unique_ptr<Operation> webMethodHandle ) {
	//This removes the Class:: part of Macro registration
	size_t pos = operationName.rfind("::");
	string filteredName(pos != operationName.npos ? operationName.substr(pos + 2) : operationName);
	_map[serviceName + string("/") + filteredName] = move(webMethodHandle);
}

//TODO: fix this
OperationStore& OperationStore::Instance() {
	static OperationStore inst;
	return inst;
}

Operation& OperationStore::operation(const string& serviceName, const string& operationName) const {
	map<string, unique_ptr<Operation> >::const_iterator it = _map.find(serviceName + "/" + operationName);
	if (it == _map.end()) {
		throw runtime_error("Unregistered web method requested : [" + serviceName + "::" + operationName + ']');
	}
	return *it->second;
}

}