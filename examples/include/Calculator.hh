#ifndef _CALCULATOR_HH_
#define _CALCULATOR_HH_
#include <answer/OperationStore.hh>
#include <string>
#include <set>

namespace Webservice{

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
		CalculatorResponse calculator(const CalculatorRequest &request){
			CalculatorResponse response;
			switch(request.operation){
				case 'x':
				case '*':
					response.result = request.operand1 * request.operand2;
					break;
				case '-':
					response.result = request.operand1 - request.operand2;
					break;
				case 'd':
				case '/':
					response.result = request.operand1 / request.operand2;
					break;
				case '+':
					response.result = request.operand1 + request.operand2;
					break;
				default:
					throw answer::WebMethodInvalidInput("Invalid operation requested");
			}
		}
};

REGISTER_OPERATION(MyService::calculator);

}

#endif // _CALCULATOR_HH_
