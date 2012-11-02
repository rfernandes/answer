#ifndef _AXIS_PROVIDERSTORE_FACILITY_HH_
#define _AXIS_PROVIDERSTORE_FACILITY_HH_

#include "answer/Environment.hh"

#include <axis2_msg_ctx.h>
#include <axis2_op_ctx.h>

namespace answer{
	namespace adapter{
		namespace axis{
			class AxisProviderStoreFacility: public ProviderStore{
				const axutil_env_t * _env;
				axis2_ctx_t* _ctx;
			public:
				AxisProviderStoreFacility(const axutil_env_t * env, struct axis2_msg_ctx * msg_ctx);
				virtual void addProvider(const std::string& key, const boost::any& value);
				virtual const boost::any& getProvider(const std::string& key) const;
				virtual bool contains(const std::string& key) const;
			};
		}
	}
}

#endif //_AXIS_PROVIDERSTORE_FACILITY_HH_