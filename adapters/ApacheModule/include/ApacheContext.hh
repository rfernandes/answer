#ifndef _APACHE_CONTEXT_HH_
#define _APACHE_CONTEXT_HH_

#include "answer/Context.hh"
#include "ApacheOperationInfo.hh"
#include <httpd.h>
#include "ApacheModule.hh"

namespace answer{
namespace adapter{
namespace apache{

  class ApacheTransport: public TransportInfo{
    std::list< std::string > _accepts;
  public:

    ApacheTransport(request_rec* r, const answer_conf_t& conf);
    
    virtual const std::list< std::string >& accepts() const;
    virtual const std::string& redirect() const;
    virtual const std::string& redirect(const std::string& uri);
    virtual bool redirectSet() const;
  };

  class ApacheContext: public Context {
  
    void queryRequestFormat(apreq_handle_t* apr);
    void axisRequestFormat(request_rec* r);

    ApacheTransport _transport;
    ApacheOperationInfo _operation;

  public:
    ApacheContext (request_rec* r, const answer_conf_t& conf);
    
    ~ApacheContext();

    virtual OperationInfo& operationInfo(){
      return _operation;
    }
    virtual TransportInfo& transportInfo(){
      return _transport;
    }
  };

} //apache
} //adapter
} //answer

#endif //_APACHE_CONTEXT_HH_