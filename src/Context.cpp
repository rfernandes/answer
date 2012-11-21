#include "answer/Context.hh"

namespace answer
{

	std::string ResponseContext::kWildcardMime = "*/*";
	std::string ResponseContext::kXmlMimeVariant1 = "text/xml";
	std::string ResponseContext::kXmlMimeVariant2 = "application/xml";
	
	const std::list< std::pair< std::string, std::string > >& ResponseContext::getAdditionalHeaders() const {
		return _additional_headers;
	}
	
	const std::string& ResponseContext::getContentType() const
	{
		return _content_type;
	}
	
	void ResponseContext::setLocationResponse ( const std::string& url ) 
	{
		_additional_headers.push_back(std::make_pair("Location",url));
		_use_raw = true;
		_is_location = true;
	}
	
	void ResponseContext::setFileAttachmentResponse ( const std::string& content_type, const std::string& filename ) 
	{
		_content_type = content_type;
		_additional_headers.push_back(std::make_pair("Content-Disposition", "attachment; filename=\""+filename+"\""));
		_use_raw = true;
	}
	
	void ResponseContext::reset() {
		_content_type = kXmlMimeVariant1;
		_additional_headers.clear();
		_use_raw = false;
		_is_location = false;
	}
	
	bool ResponseContext::useRaw() const
	{
		return _use_raw;
	}
	
	bool ResponseContext::isLocationResponse() const
	{
		return _is_location;
	}
	
	const std::string& ResponseContext::getWildcardMime() const
	{
		return kWildcardMime;
	}
	
	ResponseContext::ResponseContext() : _content_type ( kXmlMimeVariant1 ), _use_raw ( false ), _is_location ( false ) {}
	
	void ResponseContext::addCookie(const Cookie& cookie) 
	{
		_additional_headers.push_back(std::make_pair("Set-Cookie", cookie.toString() ));
	}
	
	Context& Context::getInstance()
	{
		static Context ctx;
		return ctx;
	}
	
	void Context::reset() {
		_response.reset();
		_request.reset();
	}
	
	RequestContext& Context::request()
	{
		return _request;
	}

	ResponseContext& Context::response()
	{
		return _response;
	}

	void RequestContext::reset() {
		_accept_headers_list.clear();
		//TODO: reset url?
	}

	void RequestContext::addAccept ( const std::string& accept_str ) {
		_accept_headers_list.push_back(accept_str);
	}

	void RequestContext::setAcceptList ( const std::list< std::string >& accept_list ) {
		_accept_headers_list = accept_list;
	}
	
	const std::list< std::string >& RequestContext::getAcceptList()
	{
		return _accept_headers_list;
	}
	
	
	void RequestContext::setEndPointURL(const std::string& url) {
		_endpoint_url = url;	
	}
	
	const std::string & RequestContext::getEndPointURL() const {
		return _endpoint_url;
	}

}

