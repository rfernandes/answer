#include "FCGIOperationInfo.hh"

#include <exception>

using namespace std;

namespace answer{
namespace adapter{
namespace fcgi{

FCGIOperationInfo::FCGIOperationInfo(const Fastcgipp::Http::Environment& env)
{
  //Get service and operation values, if available
  map< string, string >::const_iterator it;

  _service = env.findGet("service");
  _operation = env.findGet("operation");
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
