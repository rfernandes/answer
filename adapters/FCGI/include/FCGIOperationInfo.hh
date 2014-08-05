#ifndef _FCGI_OPERATION_INFO_HH_
#define _FCGI_OPERATION_INFO_HH_
#include "answer/Environment.hh"
#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>

namespace answer
{
namespace adapter
{
namespace fcgi
{

class FCGIOperationInfo: public OperationInfo
{
  std::string _service;
  std::string _operation;
  std::string _url;
public:
  FCGIOperationInfo(const Fastcgipp::Http::Environment &env);

  virtual const std::string &operation() const;
  virtual const std::string &service() const;
  virtual const std::string &url() const;

};

}
}
}

#endif //_FCGI_OPERATION_INFO_HH_
