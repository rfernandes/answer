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
void responsePart(Response &ret)
{

  const answer::Context::Accepts &accept_headers_list = answer::Context::Instance().accepts();
  answer::Context::Accepts::const_iterator it = accept_headers_list.begin();

  std::ostringstream encodedReponse;

  for (; it != accept_headers_list.end(); ++it)
  {
    // Search for custom codecs
    if (codec::Codec(encodedReponse, *it))
    {
      if (*it != "*/*")  // Otherwise the Codec should set the contentType via the context
      {
        ret.contentType(*it);
      }

      break;
    }
  }

  // Use a generic codec
  if (it == accept_headers_list.end())
  {
    for (it = accept_headers_list.begin(); it != accept_headers_list.end(); ++it)
    {
      if (codec::GenericCodec(encodedReponse, *it))
      {
        ret.contentType(*it);
        break;
      }
    }
  }

  if (it == accept_headers_list.end())
  {
    throw std::runtime_error("No appropriate codec available");
  }

  // TODO: Rework this interface
  //  If void is overwritten and returns false, use the default
  //  implementation (Assumes no char template is defined)
  std::ostringstream wrappedReponse;

  if (!codec::ResponseWrapper<void>(wrappedReponse, encodedReponse.str(), ret.contentType(), nullptr))
  {
    codec::ResponseWrapper<char>(wrappedReponse, encodedReponse.str(), ret.contentType(), nullptr);
  }

  ret.body(wrappedReponse.str());
}

}