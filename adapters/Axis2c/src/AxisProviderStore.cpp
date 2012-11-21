#include "AxisProviderStore.hh"
#include <stdexcept>

namespace answer{
	namespace adapter{
		namespace axis{

AxisProviderStoreFacility::AxisProviderStoreFacility(const axutil_env_t * env, axis2_msg_ctx * msg_ctx):_env(env){
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


const boost::any& AxisProviderStoreFacility::getProvider(const std::string &key) const{
	axutil_property_t *property = NULL;
	property = axis2_ctx_get_property(_ctx, _env, key.c_str());
	if (!property){
// 		std::cerr << "No property " << key << " found" << std::endl;
		throw std::runtime_error("No provider [" + key + "]");
	}
	boost::any *anyRet(static_cast<boost::any*>(axutil_property_get_value(property, _env)));
// 	std::cerr << "Trying to cast "<< (boost::any_cast<RBAC::PDP *>(*anyRet))->getRole("domain_admin").toString() << std::endl;
	return *anyRet;
}

void axis2_single_type_delete_any(void *obj_to_be_freed, const axutil_env_t */*env*/)
{
	delete static_cast<boost::any*>(obj_to_be_freed);
}

void AxisProviderStoreFacility::addProvider(const std::string& key, const boost::any& value)
{
	boost::any *test = new boost::any(value);
	//Create and insert the property
	axutil_property_t *property = NULL;
	property = axutil_property_create(_env);

	axutil_property_set_scope(property, _env, AXIS2_SCOPE_REQUEST);
	axutil_property_set_value(property, _env, static_cast<void*> (test));

	axutil_property_set_free_func(property, _env, axis2_single_type_delete_any);

	axis2_ctx_set_property(_ctx, _env, strdup(key.c_str()), property);
}

bool AxisProviderStoreFacility::contains(const std::string& key) const
{
	return axis2_ctx_get_property(_ctx, _env, key.c_str());
}


}
}
}