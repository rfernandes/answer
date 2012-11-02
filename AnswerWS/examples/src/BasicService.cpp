#include "include/BasicService.hh"

std::set<std::string> BasicService::simple_operation(){
	std::set<std::string> ret;

	ret.insert("Ennie");
	ret.insert("Meenie");
	ret.insert("Miney");
	ret.insert("Moe");

	return ret;
}

void BasicService::hello_world(){
	return "Hello World";
}