#ifndef _BASIC_SERVICE_HH_
#define _BASIC_SERVICE_HH_
#include <answer/Operation.hh>
#include <string>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>

bool responseWrapper(std::ostream &out, const std::string &response, const std::string &mimetype, const answer::WebMethodException *ex)
{
  if (mimetype != "application/xml") return false;

  out << "<return>"
      << response
      << "<status>"
      << "<status_message>" << (ex ? ex->what() : "OK")  << "</status_message>"
      << "</status>"
      << "</return>";
  return true;
}

struct BasicOperationResponse
{
  std::list<std::string> items;

  template<class Archive>
  void serialize(Archive &ar, const unsigned int /*version*/)
  {
    ar & BOOST_SERIALIZATION_NVP(items);
  }
};

struct testInput
{
  std::set<unsigned> test;

  template<class Archive>
  void serialize(Archive &ar, const unsigned int /*version*/)
  {
    ar & BOOST_SERIALIZATION_NVP(test);
  }
};

class BasicService
{
public:
  ~BasicService()
  {
    std::cerr << "Bye" << std::endl;
  }
  BasicService()
  {
    std::cerr << "Hi" << std::endl;
  }

  std::string hello_world();
  void testOperation(const testInput &test);
  BasicOperationResponse simple_operation();
};


ANSWER_RESPONSE_WRAPPER(responseWrapper)
ANSWER_REGISTER(BasicService::hello_world, "examples")
ANSWER_REGISTER(BasicService::simple_operation, "examples")
ANSWER_REGISTER(BasicService::testOperation, "examples")

#endif //_BASIC_SERVICE_HH_
