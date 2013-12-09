#include "FCGIContext.hh"

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace answer{
namespace adapter{
namespace fcgi{

using namespace std;
using namespace Fastcgipp;

FCGIContext::FCGIContext(const Fastcgipp::Http::Environment &env):
  _operation(env)
{
  _query = env.gets;
  _environment.insert(env.environment.begin(), env.environment.end());
  
  boost::split(_accepts, env.acceptContentTypes, boost::is_any_of(","), boost::token_compress_on);
  // discards "quality"
  for (vector< string >::iterator itr = _accepts.begin(); itr != _accepts.end(); ++itr) {
    size_t pos = itr->find(";");
    if (pos != string::npos)
      *itr = itr->substr(0,pos);
  }
  
  boost::property_tree::ptree pt;
  if(!env.posts.empty()){
    for(const auto &envPost: env.posts){
      if(envPost.second.type== Http::Post::form){
        pt.put(_operation.operation() + "." + envPost.first, envPost.second.value);
      }
    }
  }
  stringstream ss;
  boost::property_tree::write_xml(ss, pt);
  // Remove the xml header, header is always present
  Request req;
  req.body(ss.str().substr(ss.str().find_first_of("\n") + 1));
  request(req);
}

OperationInfo &FCGIContext::operationInfo()
{
  return _operation;
}

} //fcgi
} //adapter
} //answer
