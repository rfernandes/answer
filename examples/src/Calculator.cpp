#include "../include/Calculator.hh"

using namespace std;
using namespace answer;

AnubisResponse<CalculatorResponse> MyService::calculator(const CalculatorRequest &request){
	AnubisResponse<CalculatorResponse> response;
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
			throw WebMethodInvalidInput("Invalid operation requested");
	}
	
	return response;
}