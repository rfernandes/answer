#ifndef _FCGI_CONTEXT_HH_
#define _FCGI_CONTEXT_HH_

#include "answer/Context.hh"
#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>

namespace answer
{
namespace adapter
{
namespace fcgi
{

class FCGIContext: public Context
{
  OperationInfo _operation;

public:
  FCGIContext(const Fastcgipp::Http::Environment &env);
  virtual OperationInfo &operationInfo();
};

} //fcgi
} //adapter
} //answer

#endif //_FCGI_CONTEXT_HH_
