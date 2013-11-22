#include "FCGIContext.hh"

#include <boost/algorithm/string.hpp>

namespace answer{
namespace adapter{
namespace fcgi{

using namespace std;

FCGIContext::FCGIContext(const Fastcgipp::Http::Environment &env):
  _operation(env)
{
  _environment.insert(env.environment.begin(), env.environment.end());
  
  boost::split(_accepts, env.acceptContentTypes, boost::is_any_of(","), boost::token_compress_on);
  // discards "quality"
  for (vector< string >::iterator itr = _accepts.begin(); itr != _accepts.end(); ++itr) {
    size_t pos = itr->find(";");
    if (pos != string::npos)
      *itr = itr->substr(0,pos);
  } 
}

OperationInfo &FCGIContext::operationInfo()
{
  return _operation;
}

} //fcgi
} //adapter
} //answer
