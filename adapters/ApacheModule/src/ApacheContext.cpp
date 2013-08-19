#include <ApacheContext.hh>

#include <boost/algorithm/string.hpp>
#include <apreq.h>
#include <apreq_module.h>
#include <apreq_param.h>
#include <apreq_cookie.h>
#include <apreq_parser.h>
#include <apreq_error.h>
#include <apreq_util.h>
#include <apr_uri.h>

#include <exception>

using namespace std;

namespace answer{
namespace adapter{
namespace apache{
 
  ApacheTransport::ApacheTransport(request_rec* r, const answer_conf_t& conf){
    //TODO: Read this off of request struct
    _accepts.push_back("application/xml");
  }

  ApacheContext::ApacheContext(request_rec* r, const answer_conf_t& conf):
    _transport(r,conf),
    _operation(r,conf)
  {
    cerr << "Initing context" << endl;
   _context = this;
  }

  ApacheContext::~ApacheContext()
  {
  }

  const list< string >& ApacheTransport::accepts() const
  {
    return _accepts;
  }

  void ApacheTransport::addHeader(const string& key, const string& value, bool replace)
  {

  }

  const string& ApacheTransport::redirect() const
  {

  }

  const string& ApacheTransport::redirect(const string& uri)
  {

  }

  bool ApacheTransport::redirectSet() const
  {

  }

} //apache
} //adapter
} //answer
