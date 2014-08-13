#define BOOST_TEST_MODULE RegistrationTest
#include <boost/test/included/unit_test.hpp>

#include "answer/Operation.hh"
#include <boost/algorithm/string.hpp>

using namespace std;

class Operation
{
public:
  int test(string X)
  {
    return X.size();
  }
};

ANSWER_REGISTER(Operation::test, "Registration")

BOOST_AUTO_TEST_CASE(registration)
{
  const auto operations = answer::OperationStore::Instance().operationList();
  BOOST_CHECK(operations.front() == string("Registration/test"));
}

BOOST_AUTO_TEST_CASE(request_response)
{
  answer::Operation &operation = answer::OperationStore::Instance().operation("Registration", "test");
  answer::Response response = operation.invoke("<test>foobar</test>", {"application/xml"});
  string body = response.body();
  boost::trim(body);
  BOOST_CHECK(body == "<test>6</test>");
}

BOOST_AUTO_TEST_CASE(request_response_codec)
{
  answer::Operation &operation = answer::OperationStore::Instance().operation("Registration", "test");
  answer::Response response = operation.invoke("<test>foobar</test>", {"application/json"});
  string body = response.body();
  cerr << body << endl;
  boost::trim(body);
  BOOST_CHECK(body == "\"6\"");
}

