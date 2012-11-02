#ifndef WPSMODULEAUDIT_HH
#define WPSMODULEAUDIT_HH

#include <anubis/webservice/WebModule.hh>
#include <anubis/webservice/Params.hh>

#include <libmemcached/memcached.hpp>

namespace anubis{
namespace webservice{
namespace webmodule{

	class Audit: public answer::WebModule{
    memcache::Memcache _memcacheServer;
	public:
		Audit();
		virtual FlowStatus inFlow(answer::InFlowContext &context);
		virtual FlowStatus outFlow(answer::OutFlowContext &context);
		virtual FlowStatus outFlowFault(answer::OutFlowContext &context);

	};
}
}
}

#endif // WPSMODULEAUDIT
