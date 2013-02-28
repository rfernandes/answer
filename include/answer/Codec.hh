#ifndef _CODECSTORE_HH_
#define _CODECSTORE_HH_

#include <string>
#include <iostream>
#include "archive/ws_xml_oarchive.hpp"
#include "Exception.hh"

namespace answer{
	namespace codec{
		template<typename T>
		bool Codec ( std::ostream&, const std::string&, const T&)
		{
			return false;
		}
		
		//TODO: Move this to impl
		template<typename T>
		void GenericCodec ( std::ostream& out, const T& data)
		{
			std::cerr << "Using default codec" << std::endl;
			answer::archive::ws_xml_oarchive outA ( out );
			outA << data;
		}

		template<typename T>
		void ResponseWrapper( std::ostream& out, const std::string& response, const answer::WebMethodException*)
		{
			std::cerr << "Using default wrapper" << std::endl;
			out << response;
		}
		
	}
}

#define ANSWER_RESPONSE_WRAPPER(WrapperFunction)\
namespace answer{\
	namespace codec{\
		template<>\
		void ResponseWrapper<void>( std::ostream& out, const std::string& response, const answer::WebMethodException* ex){\
			WrapperFunction(out, response, ex);\
		}\
	}\
}\

#endif // _CODECSTORE_HH_