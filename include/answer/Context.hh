#ifndef _CONTEXT_HH_
#define _CONTEXT_HH_

#include <string>
#include <list>
#include "Environment.hh"
#include "Cookie.hh"

namespace answer {

	class Context{
	private:
		static Context *_context;
	protected:
		Context();
	public:
		virtual Environment& environment()=0;
		virtual ProviderStore& providerStore()=0;
		virtual OperationInfo& operationInfo()=0;
		virtual CookieJar& cookieJar()=0;
		virtual TransportInfo& transportInfo()=0;
		
		static Context& Instance();
	};

}

#endif // _CONTEXT_HH_
