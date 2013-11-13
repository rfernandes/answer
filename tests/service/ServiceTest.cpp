#define BOOST_TEST_MODULE RegistrationTest
#include <boost/test/included/unit_test.hpp>

//Normally this is defined in the build file, it's usually the project name
#define ANSWER_SERVICE_NAME "RegistrationTest"

#include "answer/Operation.hh"
#include <boost/algorithm/string.hpp>

using namespace std;

class Operation{
public:
	int test(string X){
		return X.size();
	}
};

ANSWER_REGISTER_OPERATION(Operation::test)

class TestContext: public
		answer::Context,
		answer::TransportInfo,
		answer::OperationInfo{
	
	string _service;
	string _operation;
	string _accept;
	list<string> _accepts;
	
public:
	TestContext(const string &serviceName, const string &operationName, const string& acceptsType):
		_service(serviceName), _operation(operationName), _accepts({acceptsType}){
	}
	
	//Context
	virtual answer::CookieJar& cookieJar(){
		throw std::runtime_error("CookieJar not implemented");
	}
	virtual answer::OperationInfo& operationInfo(){
		return *this;
	}
	virtual answer::TransportInfo& transportInfo(){
		return *this;
	}
	
	//TransportInfo
	virtual const list<string>& accepts() const{
		return _accepts;
	}
	virtual void addHeader ( const string& /*key*/, const string& /*value*/ = "", bool /*replace*/ = true ){
		throw std::runtime_error("addHeader not implemented");
	}
	virtual const string& redirect() const{
		throw std::runtime_error("redirect not implemented");
	}
	virtual const string& redirect ( const string& /*uri*/ ){
		throw std::runtime_error("redirect (set) not implemented");
	}
	virtual bool redirectSet() const{
		return false;
	}

	//OperationInfo
	virtual const string& operation() const{
		return _operation;
	}
	virtual const string& service() const{
		return _service;
	}
	virtual const string& url() const{
		throw std::runtime_error("url not implemented");
	}
};


BOOST_AUTO_TEST_CASE( registration )
{
  list<string> operations = answer::OperationStore::Instance().operationList();
	BOOST_CHECK(operations.front() == string("RegistrationTest/test"));
}

BOOST_AUTO_TEST_CASE( request_response ){
	TestContext context("RegistrationTest", "test", "application/xml");
	answer::Operation &operation = answer::OperationStore::Instance().operation("RegistrationTest", "test");
	answer::Response response = operation.invoke("<test>foobar</test>");
	string body = response.body();
	boost::trim(body);
	BOOST_CHECK(body == "<test>6</test>");
}

BOOST_AUTO_TEST_CASE( request_response_codec ){
	TestContext context("RegistrationTest", "test", "application/json");
	answer::Operation &operation = answer::OperationStore::Instance().operation("RegistrationTest", "test");
	answer::Response response = operation.invoke("<test>foobar</test>");
	string body = response.body();
	boost::trim(body);
	BOOST_CHECK(body == "\"6\"");
}

