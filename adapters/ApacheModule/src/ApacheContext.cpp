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
    // Read the accepts
    apr_table_do(
      [](void *acceptsRaw, const char* key, const char* value) -> int {
        list<string>& accepts = *static_cast<list<string>*>(acceptsRaw);
        if (!strncmp(key, "Accept", sizeof("Accept"))){
          boost::split(accepts, value, boost::is_any_of(","), boost::token_compress_on);
          // remove "quality"
          for (list< string >::iterator itr = accepts.begin(); itr != accepts.end(); ++itr) {
            size_t pos = itr->find(";");
            if (pos != string::npos)
              *itr = itr->substr(0,pos);
          }
          return 0; //done
        }
        return 1;
      },
      &_accepts, r->headers_in, NULL
    );
  }

  ApacheContext::ApacheContext(request_rec* r, const answer_conf_t& conf):
    _transport(r,conf),
    _operation(r,conf)
  {
  }

  ApacheContext::~ApacheContext()
  {
  }

  const list< string >& ApacheTransport::accepts() const
  {
    return _accepts;
  }

//   void ApacheTransport::addHeader(const string& key, const string& value, bool replace)
//   {
//     throw std::runtime_error("Apache addHeader");
//   }

  const string& ApacheTransport::redirect() const
  {
    throw std::runtime_error("Apache redirect");
  }

  const string& ApacheTransport::redirect(const string& uri)
  {
    throw std::runtime_error("Apache redirect");
  }

  bool ApacheTransport::redirectSet() const
  {
    return false;
  }

} //apache
} //adapter
} //answer
