#ifndef AXISENVIRONMENTWRAPPER_H
#define AXISENVIRONMENTWRAPPER_H

#include "answer/Environment.hh"

#include <string>
#include <axis2_msg_ctx.h>
#include <map>
#include <stdexcept>

namespace answer{
	namespace adapter{
		namespace axis{

class AxisEnvironmentWrapper: public Environment{
	const axutil_env_t * _env;
	axis2_ctx_t* _ctx;
public:
	AxisEnvironmentWrapper(const axutil_env_t * env, struct axis2_msg_ctx * msg_ctx);
	virtual void insert(const std::string&key, const std::string &value);
	virtual std::string at(const std::string&key) const;
	virtual bool contains(const std::string& key) const;
};


		}
	}
}

#endif // AXISENVIRONMENTWRAPPER_H
