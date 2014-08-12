#include "Calculator.hh"

using namespace std;
using namespace answer;

CalculatorResponse MyService::calculator(const CalculatorRequest &request)
{
  CalculatorResponse response;
  switch (request.operation)
  {
    case '*':
      response.result = request.operand1 * request.operand2;
      break;
    case '-':
      response.result = request.operand1 - request.operand2;
      break;
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
