#include "answer/Exception.hh"


using namespace std;

namespace answer
{

WebMethodException::WebMethodException(const std::string &message)
  : std::runtime_error(message) { }


WebMethodInvalidInput::WebMethodInvalidInput(const std::string &message)
  : WebMethodException(message) { }

WebMethodMissingParameter::WebMethodMissingParameter(const std::string &message)
  : WebMethodException(message) { }

}
