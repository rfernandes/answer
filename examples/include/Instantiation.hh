#ifndef _CALCULATOR_HH_
#define _CALCULATOR_HH_
#include <answer/Operation.hh>
#include <string>
#include <set>

///Define request and response types
struct Data
{
  double result;

  template<class Archive>
  void serialize(Archive &ar, const unsigned int /*version*/)
  {
    ar &BOOST_SERIALIZATION_NVP(result);
  }
};

///Complex type service testing
class Instantiation: public answer::instantiation::Singleton
{
public:
  Data calculator(const Data &request);
};

///Complex type service testing
class Instantiation
{
public:
  Data calculator(const Data &request);
};

ANSWER_REGISTER(MyService::calculator, "examples")

#endif // _CALCULATOR_HH_
