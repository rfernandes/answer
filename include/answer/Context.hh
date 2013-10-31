#ifndef _CONTEXT_HH_
#define _CONTEXT_HH_

#include <string>
#include <list>
#include "Environment.hh"
#include "Cookie.hh"

namespace answer {

	class Context{
    static Context *_context;

    Response _response;
	protected:
		Context();
	public:
    virtual ~Context();
		virtual OperationInfo& operationInfo()=0;
    virtual Environment& environment()=0;
		virtual CookieJar& cookieJar()=0;
		virtual TransportInfo& transportInfo()=0;

    //TODO: add Request
    Response& response();
    void response(const Response &response);

		static Context& Instance();
	};

}

#endif // _CONTEXT_HH_
