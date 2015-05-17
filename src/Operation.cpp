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
  _map.emplace(serviceName + string("/") + filteredName, move(webMethodHandle));
}

OperationStore &OperationStore::Instance()
{
  static OperationStore inst;
  return inst;
}

Operation &OperationStore::operation(const string &serviceName, const string &operationName) const
{
  return *_map.at(serviceName + "/" + operationName);
}

void responsePart(answer::Response &ret, const string &operationName, const answer::Context::Accepts &accepts)
{
  ostringstream encodedReponse;

  // Use generic codec
  for (const auto & accept : accepts)
  {
    if (codec::GenericEncoder(encodedReponse, accept, operationName))
    {
      ret.contentType(accept);
      break;
    }
  }

  if (ret.contentType().empty())
    throw runtime_error("No appropriate codec available");

  // TODO: Rework this interface
  //  If void is overwritten and returns false, use the default
  //  implementation (Assumes no char template is defined)
  ostringstream wrappedReponse;
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
    ostringstream wrappedReponse;
    if (!codec::ResponseWrapper<void>(wrappedReponse, "", ret.contentType(), &ex))
    {
      codec::ResponseWrapper<char>(wrappedReponse, "", ret.contentType(), &ex);
    }
    ret.body(wrappedReponse.str());
    cerr << "WebException: " << ex.what() << endl;
  }
  catch (const exception &ex)
  {
    ret.status(Response::Status::INTERNAL_SERVER_ERROR);
    cerr << "Exception: " << ex.what() << endl;
  }
  return ret;
}


}
