#include "answer/Operation.hh"

using namespace std;

namespace answer {

std::string getResponseXml(int status_code, const std::string &status, const std::string& response){
	std::stringstream ret;
		ret << "<return>"
		<< response
		<< "<status>"
		<< "<status_code>" << status_code << "</status_code>"
		<< "<status_message>" << status << "</status_message>"
		<< "</status>"
		<< "</return>";
	return ret.str();
}

}