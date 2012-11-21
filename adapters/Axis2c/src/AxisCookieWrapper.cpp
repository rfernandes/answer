#include "AxisCookieWrapper.hh"

#include <axis2_handler.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <axutil_hash.h>
#include <axis2_http_header.h>
#include <axiom_element.h>

namespace answer{

	AxisCookieWrapper::AxisCookieWrapper(const axutil_env_t * env, struct axis2_msg_ctx * msg_ctx){
		axutil_hash_t* request_headers = axis2_msg_ctx_get_transport_headers(msg_ctx, env);
		if (!request_headers){
			//TODO: provide a diferent
// 			std::cerr << "No request headers found :(" << std::endl;
			return;
		}
		axis2_http_header_t* cookie_http_header = (axis2_http_header_t *) axutil_hash_get(request_headers, "Cookie", AXIS2_HASH_KEY_STRING);
		if (cookie_http_header)
		{
			std::stringstream ss;
			ss << ' ' <<axis2_http_header_get_value (cookie_http_header, env);
			std::string keyValuePair;
			while (std::getline(ss, keyValuePair, ';')){
				std::size_t pos = keyValuePair.find('=',2);
				if (pos != std::string::npos){
					//TODO: url unescape these values
					_cookies[keyValuePair.substr(1,pos-1)] = keyValuePair.substr(pos + 1);
				}
			}
		}
	}

	void AxisCookieWrapper::insert(const std::string& /*key*/, const std::string& /*value*/)
	{

	}

	std::string AxisCookieWrapper::at(const std::string& key) const
	{
		return _cookies.at(key);
	}

	bool AxisCookieWrapper::contains(const std::string& key) const
	{
		return _cookies.count(key);
	}

}