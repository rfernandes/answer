#include "answer/Operation.hh"

using namespace std;

namespace answer
{

OperationStore::OperationStore() {}

list< string > OperationStore::operationList()
{
  list< string > ret;
  for (const auto & item : _map)
  {
    ret.push_back(item.first);
  }
  return ret;
}

void OperationStore::registerOperation(const string &serviceName, const string &operationName, unique_ptr<Operation> webMethodHandle)
{
  //This removes the Class:: part of Macro registration
  size_t pos = operationName.rfind("::");
  string filteredName(pos != operationName.npos ? operationName.substr(pos + 2) : operationName);
  _map[serviceName + string("/") + filteredName] = move(webMethodHandle);
}

OperationStore &OperationStore::Instance()
{
  static OperationStore inst;
  return inst;
}

Operation &OperationStore::operation(const string &serviceName, const string &operationName) const
{
  map<string, unique_ptr<Operation> >::const_iterator it = _map.find(serviceName + "/" + operationName);
  if (it == _map.end())
  {
    throw runtime_error("Unregistered web method requested : [" + serviceName + "::" + operationName + ']');
  }
  return *it->second;
}
void responsePart(answer::Response &ret, const string &operationName, const answer::Context::Accepts &accepts)
{
  std::ostringstream encodedReponse;

  // Use specific codec if available
  for (const auto & accept : accepts)
  {
    if (codec::Codec(encodedReponse, accept))
    {
      if (accept != "*/*")   // Otherwise the Codec should set the contentType via the context
      {
        ret.contentType(accept);
      }
      break;
    }
  }

  // Use generic codec
  for (const auto & accept : accepts)
  {
    if (codec::GenericCodec(encodedReponse, accept, operationName))
    {
      ret.contentType(accept);
      break;
    }
  }

  if (ret.contentType().empty())
    throw std::runtime_error("No appropriate codec available");

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
