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
#include <axis2_util.h>
#include <axis2_http_transport_utils.h>

namespace answer{
	namespace adapter{
		namespace axis{

	AxisCookieJarWrapper::AxisCookieJarWrapper(const axutil_env_t * env, struct axis2_msg_ctx * msg_ctx){
		axutil_hash_t* request_headers = axis2_msg_ctx_get_transport_headers(msg_ctx, env);
		if (!request_headers){
			std::cerr << "Could not get request headers, check if transportIn reciever defines exposeHeaders=true parameter" << std::endl;
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
// 					std::cerr << "Cookie found: " << (std::string)cookie << std::endl;
					insert(cookie, false);
				}
			}
		}
	}

	void AxisCookieJarWrapper::insert( const answer::Cookie& cookie, bool isNew )
	{
		// always smash existing cookie. this is the correct behaviour because all changes are made by the ws themselves
		CookieContext c(cookie,isNew);
		std::map< std::string, CookieContext >::iterator itr = _cookies.find(cookie.name());
		if (itr != _cookies.end())
			itr->second = c;
		else
			_cookies.insert( make_pair(cookie.name(), c) );
	}

	const Cookie& AxisCookieJarWrapper::at(const std::string& cookieName) const
	{
		return _cookies.at(cookieName).cookie;
	}

	std::list< Cookie > AxisCookieJarWrapper::list( bool onlyNew ) const
	{
		std::list< Cookie > lst;
		for (std::map< std::string, CookieContext >::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it){
// 			std::cerr << "name=["<<it->first<<"] isnew=["<<it->second.isNew<<"] value=["<<(std::string)it->second.cookie<<"]" << std::endl;
			if (onlyNew && !it->second.isNew)
				continue;
			lst.push_back(it->second.cookie);
		}
		return lst;
	}

	void AxisCookieJarWrapper::remove(const std::string& cookieName)
	{
		std::map< std::string, CookieContext >::iterator it = _cookies.find(cookieName);
		if (it != _cookies.end()){
			Cookie::Expires two_years_ago(boost::posix_time::second_clock::local_time() - boost::gregorian::years(2));
			it->second.cookie.expires(two_years_ago);
			it->second.isNew = true;
		}
	}

	bool AxisCookieJarWrapper::contains(const std::string& key) const
	{
		return _cookies.count(key);
	}

//Namespaces
		}
	}
}
