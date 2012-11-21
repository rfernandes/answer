#include "answer/Params.hh"
#include <iostream>
#include <algorithm>

using namespace std;

namespace answer{
	template<>
	bool Params::getParamByType(const std::string& paramName) const {
		bool ret;
		try {
			std::string param = getParam(paramName);
			std::transform(param.begin(), param.end(), param.begin(), ::tolower);
			ret = (param != "false");
		} catch (std::exception &ex) {
			throw std::runtime_error("Bool parameter type error: " +paramName);
		}
		return ret;
	}
}