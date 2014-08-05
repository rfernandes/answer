#include "BasicService.hh"

#include <boost/serialization/set.hpp>

namespace WebServices
{
BasicOperationResponse BasicService::simple_operation()
{
  BasicOperationResponse ret;

  ret.items.push_back("Ennie");
  ret.items.push_back("Meenie");
  ret.items.push_back("Miney");
  ret.items.push_back("Moe");

  return ret;
}

std::string BasicService::hello_world()
{
  return "Hello World";
}

void BasicService::testOperation(const WebServices::testInput &test)
{
  std::cerr << "Got : " << std::endl;
  std::copy(test.test.begin(), test.test.end(), std::ostream_iterator<unsigned>(std::cerr, " - "));
}

}

