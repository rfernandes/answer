#include "answer/Operation.hh"

using namespace std;

namespace answer
{

OperationStore::OperationStore() {}

vector< string > OperationStore::operationList()
{
  vector< string > ret;
  ret.reserve(_map.size());
  for (const auto & item : _map)
  {
    ret.emplace_back(item.first);
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

Response Operation::invoke(const string &params, const Context::Accepts &accepts)
{
  Response ret;
  try
  {
    process(ret, params, accepts);
  }
  catch (const WebMethodException &ex)
  {
    ret.status(Response::Status::INTERNAL_SERVER_ERROR);
    std::ostringstream wrappedReponse;
    if (!codec::ResponseWrapper<void>(wrappedReponse, "", ret.contentType(), &ex))
    {
      codec::ResponseWrapper<char>(wrappedReponse, "", ret.contentType(), &ex);
    }
    ret.body(wrappedReponse.str());
    std::cerr << "WebException: " << ex.what() << std::endl;
  }
  catch (const std::exception &ex)
  {
    ret.status(Response::Status::INTERNAL_SERVER_ERROR);
    std::cerr << "Exception: " << ex.what() << std::endl;
  }
  return ret;
}


}
