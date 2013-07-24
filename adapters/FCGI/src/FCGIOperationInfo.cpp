#include "FCGIOperationInfo.hh"

#include <exception>

namespace answer{
namespace adapter{
namespace fcgi{

FCGIOperationInfo::FCGIOperationInfo(const Fastcgipp::Http::Environment< char >& env)
{
	_service = env.gets.at("service");
	_operation = env.gets.at("operation");
}

	

const std::string& FCGIOperationInfo::getOperationName() const
{
	return _operation;
}

const std::string& FCGIOperationInfo::getServiceName() const
{
	return _service;
}

const std::string& FCGIOperationInfo::getURL() const
{
	throw std::runtime_error("Get URL Unimplemented");
}


}
}
}
