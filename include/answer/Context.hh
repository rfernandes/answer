#ifndef CONTEXT_HH
#define CONTEXT_HH

#include <string>
#include "answer/Cookie.hh"
#include "WebModule.hh"
#include <list>

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
		
		static Context& getInstance(){
      if (!_context){
        throw std::runtime_error("Uninitialized context");
      }
			return *_context;
		}
	};


}

#endif // CONTEXT_HH
