#include "FCGIContext.hh"

#include <boost/algorithm/string.hpp>

namespace answer{
namespace adapter{
namespace fcgi{

using namespace std;

FCGITransport::FCGITransport(const Fastcgipp::Http::Environment< char >& env){
  boost::split(_accepts, env.acceptContentTypes, boost::is_any_of(","), boost::token_compress_on);
  // discards "quality"
  for (list< string >::iterator itr = _accepts.begin(); itr != _accepts.end(); ++itr) {
    size_t pos = itr->find(";");
    if (pos != string::npos)
      *itr = itr->substr(0,pos);
  }
}

FCGIContext::FCGIContext(const Fastcgipp::Http::Environment< char > &env):
  _transport(env),
  _operation(env)
{
  _environment.insert(env.env.begin(), env.env.end());
}

FCGIContext::~FCGIContext()
{
}

const list< string >& FCGITransport::accepts() const
{
  return _accepts;
}

const string& FCGITransport::redirect() const
{
  throw std::runtime_error("FCGI redirect unimplemented");
}

const string& FCGITransport::redirect(const string& uri)
{
  throw std::runtime_error("FCGI redirect unimplemented");
}

bool FCGITransport::redirectSet() const
{
  return false;
}

CookieJar &FCGIContext::cookieJar()
{
  throw std::runtime_error("CookieJar unimplemented");
}
OperationInfo &FCGIContext::operationInfo()
{
  return _operation;
}
TransportInfo &FCGIContext::transportInfo()
{
  return _transport;
}

} //fcgi
} //adapter
} //answer
