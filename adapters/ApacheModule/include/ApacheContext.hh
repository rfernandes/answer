#ifndef _APACHE_CONTEXT_HH_
#define _APACHE_CONTEXT_HH_

#include <answer/Context.hh>
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
    virtual void addHeader(const std::string& key, const std::string& value = "", bool replace = true);
    virtual const std::string& redirect() const;
    virtual const std::string& redirect(const std::string& uri);
    virtual bool redirectSet() const;
  };

  class ApacheContext: public Context {
//    ApacheCookieJarWrapper _cookies;
//    ApacheEnvironmentWrapper _environment;
//    ApacheProviderStoreFacility _providerStore;
  
    void queryRequestFormat(apreq_handle_t* apr);
    void axisRequestFormat(request_rec* r);

    ApacheOperationInfo _operation;
    ApacheTransport _transport;

  public:
    ApacheContext (request_rec* r, const answer_conf_t& conf);
    
    ~ApacheContext();

    virtual CookieJar& cookieJar() {
      throw std::runtime_error("CookieJar unimplemented");
    }
    virtual Environment& environment() {
      throw std::runtime_error("Environment unimplemented");
    }
    virtual ProviderStore& providerStore() {
      throw std::runtime_error("ProviderStore unimplemented");
    }
    virtual OperationInfo& operationInfo() {
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