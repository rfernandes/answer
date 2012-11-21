#ifndef CONTEXT_HH
#define CONTEXT_HH

#include <string>
#include "answer/Cookie.hh"
#include "WebModule.hh"
#include <list>

namespace answer {
	
	class RequestContext {
	public:
		void reset();
		void addAccept(const std::string& accept_str);
		void setAcceptList(const std::list<std::string>& accept_list);
		const std::list<std::string>& getAcceptList();
	
		void setEndPointURL(const std::string& url);
		const std::string &getEndPointURL() const;
	private:
		std::list<std::string> _accept_headers_list;
		std::string _endpoint_url;
	};
	
	class ResponseContext {
	public:
		ResponseContext();
		const std::list<std::pair<std::string,std::string> >& getAdditionalHeaders() const;
		const std::string& getContentType() const;
		void setLocationResponse(const std::string& url);
		void setFileAttachmentResponse(const std::string& content_type, const std::string& filename);
		bool isLocationResponse() const;
		void reset();
		bool useRaw() const;
		const std::string& getWildcardMime() const;
		void addCookie(const Cookie& cookie);
		
	private:
		std::string _content_type;
		std::list<std::pair<std::string,std::string> >_additional_headers;
		bool _use_raw;
		bool _is_location;
		
		static std::string kWildcardMime;
		static std::string kXmlMimeVariant1;
		static std::string kXmlMimeVariant2;
	};
	
	class Context{
	private:
		Context(){}
		RequestContext _request;
		ResponseContext _response;
		
	public:
		static Context& getInstance();
		void reset();
		RequestContext& request();
		ResponseContext& response();
	};

}

#endif // CONTEXT_HH
