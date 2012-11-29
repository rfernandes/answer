#ifndef _AXISCOOKIEJARWRAPPER_HH_
#define _AXISCOOKIEJARWRAPPER_HH_

#include "answer/Environment.hh"
#include "answer/Cookie.hh"
#include <axis2_conf_ctx.h>
#include <map>


namespace answer{
	namespace adapter{
		namespace axis{

	class AxisCookieJarWrapper: public CookieJar{
		std::map<std::string, Cookie> _cookies;
	public:
		AxisCookieJarWrapper(const axutil_env_t * env, struct axis2_msg_ctx * msg_ctx);
    virtual bool contains(const std::string& cookieName) const;
    virtual const Cookie& getCookie(const std::string& cookieName) const;
    virtual void insert(const Cookie& cookie);
    virtual std::list< Cookie > list() const;
    virtual void remove(const std::string& cookieName);
	};

		}
	}
}

#endif // _AXISCOOKIEJARWRAPPER_HH_
