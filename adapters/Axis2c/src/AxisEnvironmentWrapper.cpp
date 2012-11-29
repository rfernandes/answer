#include "AxisEnvironmentWrapper.hh"

#include <axis2_handler.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <axutil_hash.h>
#include <axis2_op_ctx.h>
#include <axis2_http_header.h>
#include <axiom_element.h>



void axis2_single_type_free(void *obj_to_be_freed, const axutil_env_t *env)
{
	AXIS2_FREE(env->allocator, obj_to_be_freed);
}
void axis2_no_free(void */*obj_to_be_freed*/, const axutil_env_t */*env*/){}

#define AXIS2_MALLOC(allocator, size) \
      ((allocator)->malloc_fn(allocator, size))

namespace answer{
	namespace adapter{
		namespace axis{

AxisEnvironmentWrapper::AxisEnvironmentWrapper(const axutil_env_t * env, axis2_msg_ctx * msg_ctx):_env(env){
	axis2_op_ctx* op_ctx = axis2_msg_ctx_get_op_ctx (msg_ctx, env);
// 	_baseCtx = axis2_msg_ctx_get_base(msg_ctx, env);
	if (!op_ctx)
	{
		throw std::runtime_error("Couldn't get operation context from message context");
	}
	_ctx = axis2_op_ctx_get_base(op_ctx, env);
	if (!_ctx)
	{
		throw std::runtime_error("Couldn't get operation context from message context");
	}
}


void AxisEnvironmentWrapper::insert(const std::string&key, const std::string &value){
	//Copy the value;

	char * property_value = (char*)AXIS2_MALLOC(_env->allocator, sizeof(std::string::value_type)*value.size());
	strcpy(property_value, value.c_str());
	//Create and insert the property
	axutil_property_t *property = NULL;
	property = axutil_property_create(_env);

	axutil_property_set_scope(property, _env, AXIS2_SCOPE_REQUEST);
	axutil_property_set_value(property, _env, property_value);

	axutil_property_set_free_func(property, _env, axis2_single_type_free);

	/*axis2_status_t status = */axis2_ctx_set_property(_ctx, _env, strdup(key.c_str()), property);
}

std::string AxisEnvironmentWrapper::at(const std::string &key) const{
	axutil_property_t *property = NULL;
	property = axis2_ctx_get_property(_ctx, _env, key.c_str());
	if (!property){
// 		std::cerr << "No property " << key << " found" << std::endl;
		return std::string();
	}
	std::string ret(static_cast<char*>(axutil_property_get_value(property, _env)));
	return ret;
}

bool AxisEnvironmentWrapper::contains(const std::string& key) const
{
	return axis2_ctx_get_property(_ctx, _env, key.c_str());
}

		}
	}
}
