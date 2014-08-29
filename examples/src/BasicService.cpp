#include "BasicService.hh"

#include <boost/serialization/set.hpp>

BasicOperationResponse BasicService::simple_operation()
{
  BasicOperationResponse ret;

  ret.items = {
    "Ennie",
    "Meenie",
    "Miney",
    "Moe"
  };

  return ret;
}



std::string BasicService::hello_world()
{
  return "Hello World";
}

void BasicService::testOperation(const testInput &test)
{
  std::cerr << "Got : " << std::endl;
  std::copy(test.test.begin(), test.test.end(), std::ostream_iterator<unsigned>(std::cerr, " - "));
}
