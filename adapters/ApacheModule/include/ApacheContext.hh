#ifndef _APACHE_CONTEXT_HH_
#define _APACHE_CONTEXT_HH_

#include "answer/Context.hh"
#include "ApacheOperationInfo.hh"
#include <httpd.h>
#include "ApacheModule.hh"

namespace answer{
namespace adapter{
namespace apache{

  class ApacheContext: public Context {
  
    void queryRequestFormat(apreq_handle_t* apr);
    void axisRequestFormat(request_rec* r);

    ApacheOperationInfo _operation;

  public:
    ApacheContext (request_rec* r, const answer_conf_t& conf);
    
    ~ApacheContext();

    virtual OperationInfo& operationInfo(){
      return _operation;
    }
  };

} //apache
} //adapter
} //answer

#endif //_APACHE_CONTEXT_HH_