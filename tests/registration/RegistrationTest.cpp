#define BOOST_TEST_MODULE SerializationTest
#include <boost/test/included/unit_test.hpp>

//Normally this is defined in the build file, it's usually the project name
#define ANSWER_SERVICE_NAME "SerializationTest"

#include "answer/Operation.hh"

using namespace std;

class Operation{
public:
		int test(string X){
			return X.size();
		}
};

ANSWER_REGISTER_OPERATION(Operation::test);

BOOST_AUTO_TEST_CASE( registration )
{
  list<string> operations = answer::OperationStore::Instance().operationList();
	BOOST_CHECK(operations.front() == string("SerializationTest/test"));
}

