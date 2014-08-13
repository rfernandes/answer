#define BOOST_TEST_MODULE RegistrationTest
#include <boost/test/included/unit_test.hpp>
#include "answer/Operation.hh"
#include <boost/algorithm/string.hpp>

using namespace std;

class Operation
{  
  std::string _lastCall;
  
public:
  Operation():_lastCall("first"){}
  
protected:
  string _test(const string& currCall){
    std::string ret = _lastCall;
    _lastCall = currCall;
    return ret;
  }
};

class SingletonTest : 
  public Operation, 
  public answer::instantiation::Singleton
{
public:

  string test(const string& currCall){
    return _test(currCall);
  }
};

ANSWER_REGISTER(SingletonTest::test, "singleton")

BOOST_AUTO_TEST_CASE(singleton)
{
  const auto operations = answer::OperationStore::Instance().operationList();
  auto &operation = answer::OperationStore::Instance().operation("singleton", "test");
  
  auto response1 = operation.invoke("<test>second</test>", "", {"application/xml"}).body();
  auto response2 = operation.invoke("<test></test>", "", {"application/xml"}).body();

  boost::trim(response1);
  boost::trim(response2);
  
  BOOST_CHECK(response1 == "<test>first</test>");
  BOOST_CHECK(response2 == "<test>second</test>");
}


class SingleCall:
  public Operation,
  public answer::instantiation::SingleCall
{
public:
  string test(const string& currCall){
    return _test(currCall);
  }
};

ANSWER_REGISTER(SingleCall::test, "singlecall")

BOOST_AUTO_TEST_CASE(singlecall)
{
  const auto operations = answer::OperationStore::Instance().operationList();
  auto &operation = answer::OperationStore::Instance().operation("singlecall", "test");
  auto response1 = operation.invoke("<test>second</test>", "", {"application/xml"}).body();
  auto response2 = operation.invoke("<test>second</test>", "", {"application/xml"}).body();

  boost::trim(response1);
  boost::trim(response2);
  
  BOOST_CHECK(response1 == "<test>first</test>");
  BOOST_CHECK(response2 == "<test>first</test>");
}


