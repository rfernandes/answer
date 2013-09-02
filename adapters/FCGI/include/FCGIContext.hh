#ifndef _FCGI_CONTEXT_HH_
#define _FCGI_CONTEXT_HH_

#include <answer/Context.hh>
#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>

#include "FCGIOperationInfo.hh"

namespace answer{
namespace adapter{
namespace fcgi{

	class FCGITransport: public TransportInfo{
		std::list< std::string > _accepts;
	public:
		
		FCGITransport(const Fastcgipp::Http::Environment< char >& env);
		
		virtual const std::list< std::string >& accepts() const;
		virtual void addHeader(const std::string& key, const std::string& value = "", bool replace = true);
		virtual const std::string& redirect() const;
		virtual const std::string& redirect(const std::string& uri);
		virtual bool redirectSet() const;
	};

	class FCGIContext: public Context{
// 		FCGICookieJarWrapper _cookies;
// 		FCGIEnvironmentWrapper _environment;
// 		FCGIProviderStoreFacility _providerStore;
	
		FCGIOperationInfo _operation;
		FCGITransport _transport;

	public:
		FCGIContext (const Fastcgipp::Http::Environment< char > &env);
		
		~FCGIContext();

		virtual CookieJar& cookieJar() {
			throw std::runtime_error("CookieJar unimplemented");
		}
		virtual Environment& environment() {
			throw std::runtime_error("Environment unimplemented");
		}
		virtual ProviderStore& providerStore() {
			throw std::runtime_error("ProviderStore unimplemented");
		}
		virtual OperationInfo& operationInfo() {
			return _operation;
		}
		virtual TransportInfo& transportInfo(){
			return _transport;
		}

	};


} //fcgi
} //adapter
} //answer

#endif //_FCGI_CONTEXT_HH_