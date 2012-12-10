#include "AxisCookieWrapper.hh"

#include <axis2_handler.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <axutil_hash.h>
#include <axis2_http_header.h>
#include <axiom_element.h>
#include <axis2_msg_ctx.h>
#include <axis2_http_transport_utils.h>

namespace answer{
	namespace adapter{
		namespace axis{

	AxisCookieJarWrapper::AxisCookieJarWrapper(const axutil_env_t * env, struct axis2_msg_ctx * msg_ctx){
		axutil_hash_t* request_headers = axis2_msg_ctx_get_transport_headers(msg_ctx, env);
		if (!request_headers){
			//TODO: provide a diferent
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
					Cookie cookie(keyValuePair.substr(1,pos-1), keyValuePair.substr(pos + 1));
// 					std::cerr << "Cookie found: " << cookie.toString() << std::endl;
					insert(cookie);
				}
			}
		}
	}

	void AxisCookieJarWrapper::insert(const answer::Cookie& cookie)
	{
		_cookies.insert(make_pair(cookie.getName(),cookie));
	}

	const Cookie& AxisCookieJarWrapper::getCookie(const std::string& cookieName) const
	{
		return _cookies.at(cookieName);
	}

	std::list< Cookie > AxisCookieJarWrapper::list() const
	{
		std::list< Cookie > lst;
		for (std::map< std::string, Cookie >::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it){
			lst.push_back(it->second);
		}
		return lst;
	}

	void AxisCookieJarWrapper::remove(const std::string& cookieName)
	{
		_cookies.erase(cookieName);
	}

	bool AxisCookieJarWrapper::contains(const std::string& key) const
	{
		return _cookies.count(key);
	}

//Namespaces
		}
	}
}