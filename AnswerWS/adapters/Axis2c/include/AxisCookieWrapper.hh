#ifndef _COOKIEWRAPPER_HH_
#define _COOKIEWRAPPER_HH_

#include "answer/Environment.hh"
#include <axis2_conf_ctx.h>
#include <map>


namespace answer{

	class AxisCookieWrapper: public Environment{
		std::map<std::string, std::string> _cookies;
	public:
		AxisCookieWrapper(const axutil_env_t * env, struct axis2_msg_ctx * msg_ctx);
    virtual std::string at(const std::string& key) const;
		//TODO: we have no support for creating cookies. Either define an empty base, split r/w interfaces or throw exception
    virtual void insert(const std::string& key, const std::string& value);
    virtual bool contains(const std::string& key) const;
	};

}

#endif // COOKIEWRAPPER_H
