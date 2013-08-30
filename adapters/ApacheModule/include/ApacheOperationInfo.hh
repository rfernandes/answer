#ifndef _APACHE_OPERATION_INFO_HH_
#define _APACHE_OPERATION_INFO_HH_
#include <answer/Environment.hh>
#include <httpd.h>
#include <apreq.h>
#include <apreq_module.h>
#include "ApacheModule.hh"


namespace answer{
namespace adapter{
namespace apache{

class ApacheOperationInfo: public OperationInfo{
  std::string _service;
  std::string _operation;
  std::string _rawRequest;
  
  void queryRequestFormat(apreq_handle_t* apr);
  void axisRequestFormat(request_rec* r);
  
public:
  ApacheOperationInfo(request_rec* r, const answer_conf_t& conf);

  virtual const std::string& getRawRequest() const;
  virtual const std::string& operation() const;
  virtual const std::string& service() const;
  virtual const std::string& url() const;
};

}
}
}

#endif //_APACHE_OPERATION_INFO_HH_