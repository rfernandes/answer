#ifndef _CODEC_HH_
#define _CODEC_HH_

#include <string>
#include <iostream>
#include "archive/ws_json_oarchive.hpp"
#include "archive/ws_xml_oarchive.hpp"
#include "Exception.hh"
#include "Context.hh"

namespace answer{
	namespace codec{
		template<typename T>
		bool Codec ( std::ostream&, const std::string&, const T&)
		{
			return false;
		}

		template<typename T>
		bool GenericCodec ( std::ostream& out, const std::string& mimeType, const T& data)
		{
			const std::string &operationName = Context::Instance().operationInfo().operation();
			if (mimeType == "application/json"){
				answer::archive::ws_json_oarchive outA ( out );
				outA << data;
				return true;
			}
			if (mimeType == "application/xml" || mimeType == "*/*"){
				answer::archive::ws_xml_oarchive outA ( out );
				outA << boost::serialization::make_nvp(operationName.c_str(), data);
				return true;
			}
			return false;
		}

		template<typename T>
		bool ResponseWrapper( std::ostream& out, const std::string& response, const std::string& /*mimeType*/, const answer::WebMethodException*)
		{
      out << response;
			return true;
		}
		
	}
}

#define ANSWER_RESPONSE_WRAPPER(WrapperFunction)\
namespace answer{\
	namespace codec{\
		template<>\
		bool ResponseWrapper<void>( std::ostream& out, const std::string& response, const std::string& mimeType, const answer::WebMethodException* ex){\
			return WrapperFunction(out, response, mimeType, ex);\
		}\
	}\
}\

#endif // _CODEC_HH_