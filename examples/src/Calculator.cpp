#include "Calculator.hh"

using namespace std;
using namespace answer;

namespace WebServices
{

CalculatorResponse MyService::calculator(const CalculatorRequest &request)
{
  CalculatorResponse response;
  switch (request.operation)
  {
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

}
