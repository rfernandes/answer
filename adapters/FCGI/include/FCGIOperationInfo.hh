#ifndef _FCGI_OPERATION_INFO_HH_
#define _FCGI_OPERATION_INFO_HH_
#include <answer/Environment.hh>
#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>

namespace answer{
namespace adapter{
namespace fcgi{

class FCGIOperationInfo: public OperationInfo{
	std::string _service;
	std::string _operation;
public:
	FCGIOperationInfo(const Fastcgipp::Http::Environment< char > &env);

	virtual const std::string& getOperationName() const;
	virtual const std::string& getServiceName() const;
	virtual const std::string& getURL() const;
};



}
}
}

#endif //_FCGI_OPERATION_INFO_HH_