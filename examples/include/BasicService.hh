#ifndef _BASIC_SERVICE_HH_
#define _BASIC_SERVICE_HH_
#include <answer/OperationStore.hh>

#include <string>

struct request{
	std::string ;
};

///Basic type service testing
class BasicService{
public:
	std::string hello_service(const std::string& input);
};

REGISTER_OPERATION(BasicService::hello_service);

#endif //_BASIC_SERVICE_HH_