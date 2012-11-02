#ifndef _CALCULATOR_HH_
#define _CALCULATOR_HH_
#include <answer/OperationStore.hh>
#include <string>
#include <set>

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

template <class T>
struct AnubisResponse{
	
	struct Status{
		int status_code;
		std::string status_message;

		template<class Archive>
		void serialize(Archive &ar, const unsigned int /*version*/){
			ar & BOOST_SERIALIZATION_NVP(status_code);
			ar & BOOST_SERIALIZATION_NVP(status_message);
		}
	};
	
	Status status;
	T data;
	
	template<class Archive>
	void serialize(Archive &ar, const unsigned int /*version*/){
		ar & BOOST_SERIALIZATION_NVP(data);
		ar & BOOST_SERIALIZATION_NVP(status);
	}
};

std::string getResponseXml(int status_code, const std::string &status, const std::string& response){
	std::stringstream ret;
		ret << "<return>"
		<< response
		<< "<status>"
		<< "<status_code>" << status_code << "</status_code>"
		<< "<status_message>" << status << "</status_message>"
		<< "</status>"
		<< "</return>";
	return ret.str();
}

///Complex type service testing
class MyService{
	public:
		AnubisResponse<CalculatorResponse> calculator(const CalculatorRequest &request);
};

REGISTER_OPERATION(MyService::calculator);

#endif // _CALCULATOR_HH_
