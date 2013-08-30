#ifndef _CONTEXT_HH_
#define _CONTEXT_HH_

#include <string>
#include <list>
#include "Environment.hh"
#include "Params.hh"
#include "Cookie.hh"

namespace answer {

	class Context{
	protected:
		static Context *_context;
		Context(){};
	public:
		virtual Environment& environment()=0;
		virtual ProviderStore& providerStore()=0;
		virtual OperationInfo& operationInfo()=0;
		virtual CookieJar& cookieJar()=0;
		virtual TransportInfo& transportInfo()=0;
		
		static Context& Instance(){
      if (!_context){
        throw std::runtime_error("Uninitialized context");
      }
			return *_context;
		}
	};

}

#endif // _CONTEXT_HH_
