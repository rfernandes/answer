#ifndef _CALCULATOR_HH_
#define _CALCULATOR_HH_
#include <answer/Operation.hh>
#include <string>
#include <set>

namespace WebServices{

///Define request and response types
struct CalculatorResponse{
	double result;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/){
		ar & BOOST_SERIALIZATION_NVP(result);
	}
};

struct CalculatorRequest{
	double operand1;
	double operand2;
	char operation;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/){
		ar & BOOST_SERIALIZATION_NVP(operand1);
		ar & BOOST_SERIALIZATION_NVP(operand2);
		ar & BOOST_SERIALIZATION_NVP(operation);
	}
};

///Complex type service testing
class MyService{
	public:
		CalculatorResponse calculator(const CalculatorRequest &request);
};

ANSWER_REGISTER_OPERATION(MyService::calculator);

}

#endif // _CALCULATOR_HH_
