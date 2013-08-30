#include "FCGIOperationInfo.hh"

#include <exception>

using namespace std;

namespace answer{
namespace adapter{
namespace fcgi{

FCGIOperationInfo::FCGIOperationInfo(const Fastcgipp::Http::Environment< char >& env)
{
  //Get service and operation values, if available
  map< string, string >::const_iterator it;

  it = env.gets.find("service");
  if (it != env.gets.end())
    _service = it->second;
  it = env.gets.find("operation");
  if (it != env.gets.end())
    _operation = it->second;
  _url = env.requestUri;
}

const string& FCGIOperationInfo::operation() const
{
	return _operation;
}

const string& FCGIOperationInfo::service() const
{
	return _service;
}

const string& FCGIOperationInfo::url() const
{
  return _url;
}


}
}
}
