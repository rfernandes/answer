#ifndef _AXIS_TRANSPORT_HH_
#define _AXIS_TRANSPORT_HH_


#include "answer/Environment.hh"
#include <axis2_conf_ctx.h>

#include <map>

namespace answer{
	namespace adapter{
		namespace axis{

			class AxisTransport: public TransportInfo {
				std::string _uri;

				std::multimap<std::string, std::string> _headers;

				std::list<std::string> _accepts;
			public:
				AxisTransport(const axutil_env_t * env, struct axis2_msg_ctx * msg_ctx);
				virtual void addHeader(const std::string& key, const std::string& value = "", bool replace = true);
				virtual const std::string& redirect(const std::string& uri);
				virtual const std::string& redirect() const;
				virtual bool redirectSet() const;
				virtual const std::list< std::string >& accepts() const;
				
// 				void addAccepts(const std::string& accepts);
				const std::multimap<std::string, std::string>& getHeaders() const;
				};
		}
	}
}

#endif //_AXIS_OPERATION_HH_