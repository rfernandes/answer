#include <fstream>
#include <algorithm>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>
#include <dlfcn.h>

#include "answer/OperationStore.hh"
#include "answer/WebModule.hh"
#include "answer/Context.hh"

#include "FCGIContext.hh"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace boost::algorithm;

using namespace answer;
using namespace std;

using namespace Fastcgipp;

namespace answer{
namespace adapter{
namespace fcgi{

class FcgiAdapter: public Fastcgipp::Request<char>
{

	bool debug(const std::string& serviceRequest, const std::string& serviceResponse, const FCGIContext& context)
	{
   using Fastcgipp::HTML;
   
		out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
		
		out << "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' />";
		out << "<title>fastcgi++: Echo in UTF-8</title></head><body>";
		
		out << "<h1>Service Request</h1>";
		out << "<pre>";
		out << encoding(HTML) << serviceRequest << encoding(NONE);
		out << "</pre>";
		
		out << "<h1>Service Response</h1>";
		out << "<pre>";
		out << encoding(HTML) << serviceResponse << encoding(NONE);
		out << "</pre>";
		
		out << "<h1>Response Modifiers</h1>";
		out << "<pre>";
// 		out << "<b>Content-Type:</b> " << encoding(HTML) << response_context.getContentType() << encoding(NONE) << "<br/>";
// 		if (!response_context.getAdditionalHeaders().empty()) {
// 			out << "<b>Additional Headers:</b><br/>";
// 			for ( list< pair< string, string > >::const_iterator itr = response_context.getAdditionalHeaders().begin(); itr != response_context.getAdditionalHeaders().end(); ++itr) {
// 				out << "&nbsp&nbsp&nbsp*&nbsp" << encoding(HTML) << itr->first << ": " << itr->second << encoding(NONE) << "<br/>";
// 			}
// 		}
		out << "</pre>";
		
		// 		out << "<b>Hostname:</b> " << encoding(HTML) << environment().host << encoding(NONE) << "<br />";
		
		out << "<h1>Environment Parameters</h1>";
		out << "<p><b>FastCGI Version:</b> " << Protocol::version << "<br />";
		out << "<b>fastcgi++ Version:</b> " << version << "<br />";
		out << "<b>Hostname:</b> " << encoding(HTML) << environment().host << encoding(NONE) << "<br />";
		out << "<b>User Agent:</b> " << encoding(HTML) << environment().userAgent << encoding(NONE) << "<br />";
		out << "<b>Accepted Content Types:</b> " << encoding(HTML) << environment().acceptContentTypes << encoding(NONE) << "<br />";
		out << "<b>Accepted Languages:</b> " << encoding(HTML) << environment().acceptLanguages << encoding(NONE) << "<br />";
		out << "<b>Accepted Characters Sets:</b> " << encoding(HTML) << environment().acceptCharsets << encoding(NONE) << "<br />";
		out << "<b>Referer:</b> " << encoding(HTML) << environment().referer << encoding(NONE) << "<br />";
		out << "<b>Content Type:</b> " << encoding(HTML) << environment().contentType << encoding(NONE) << "<br />";
		out << "<b>Root:</b> " << encoding(HTML) << environment().root << encoding(NONE) << "<br />";
		out << "<b>Script Name:</b> " << encoding(HTML) << environment().scriptName << encoding(NONE) << "<br />";
		out << "<b>Request URI:</b> " << encoding(HTML) << environment().requestUri << encoding(NONE) << "<br />";
		out << "<b>Request Method:</b> " << encoding(HTML) << environment().requestMethod << encoding(NONE) << "<br />";
		out << "<b>Content Length:</b> " << encoding(HTML) << environment().contentLength << encoding(NONE) << "<br />";
		out << "<b>Keep Alive Time:</b> " << encoding(HTML) << environment().keepAlive << encoding(NONE) << "<br />";
		out << "<b>Server Address:</b> " << encoding(HTML) << environment().serverAddress << encoding(NONE) << "<br />";
		out << "<b>Server Port:</b> " << encoding(HTML) << environment().serverPort << encoding(NONE) << "<br />";
		out << "<b>Client Address:</b> " << encoding(HTML) << environment().remoteAddress << encoding(NONE) << "<br />";
		out << "<b>Client Port:</b> " << encoding(HTML) << environment().remotePort << encoding(NONE) << "<br />";
		out << "<b>If Modified Since:</b> " << encoding(HTML) << environment().ifModifiedSince << encoding(NONE) << "</p>";
		
		out << "<h1>Path Data</h1>";
		if(environment().pathInfo.size())
		{
			string preTab;
			for(Http::Environment<char>::PathInfo::const_iterator it=environment().pathInfo.begin(); it!=environment().pathInfo.end(); ++it)
			{
				out << preTab << encoding(HTML) << *it << encoding(NONE) << "<br />";
				preTab += "&nbsp;&nbsp;&nbsp;";
			}
		}
		else
			out << "<p>No Path Info</p>";
		
		out << "<h1>GET Data</h1>";
		if(environment().gets.size())
			for(Http::Environment<char>::Gets::const_iterator it=environment().gets.begin(); it!=environment().gets.end(); ++it)
				out << "<b>" << encoding(HTML) << it->first << encoding(NONE) << ":</b> " << encoding(HTML) << it->second << encoding(NONE) << "<br />";
			else
				out << "<p>No GET data</p>";
			
			out << "<h1>Cookie Data</h1>";
		if(environment().cookies.size())
			for(Http::Environment<char>::Cookies::const_iterator it=environment().cookies.begin(); it!=environment().cookies.end(); ++it)
				out << "<b>" << encoding(HTML) << it->first << encoding(NONE) << ":</b> " << encoding(HTML) << it->second << encoding(NONE) << "<br />";
			else
				out << "<p>No Cookie data</p>";
			
			out << "<h1>POST Data</h1>";
		if(environment().posts.size())
		{
			for(Http::Environment<char>::Posts::const_iterator it=environment().posts.begin(); it!=environment().posts.end(); ++it)
			{
				out << "<h2>" << encoding(HTML) << it->first << encoding(NONE) << "</h2>";
				if(it->second.type==Http::Post<char>::form)
				{
					out << "<p><b>Type:</b> form data<br />";
					out << "<b>Value:</b> " << encoding(HTML) << it->second.value << encoding(NONE) << "</p>";
				}
				
				else
				{
					out << "<p><b>Type:</b> file<br />";
					out << "<b>Filename:</b> " << encoding(HTML) << it->second.filename << encoding(NONE) << "<br />";
					out << "<b>Content Type:</b> " << encoding(HTML) << it->second.contentType << encoding(NONE) << "<br />";
					out << "<b>Size:</b> " << it->second.size() << "<br />";
					out << "<b>Data:</b></p><pre>";
					out.dump(it->second.data(), it->second.size());
					out << "</pre>";
				}
			}
		}
		else
			out << "<p>No POST data</p>";
		
		out << "<h2>Loaded Services</h2>";
		list< string > opList = OperationStore::getInstance().getOperationList();
		for (list< string >::const_iterator it = opList.begin(); it != opList.end(); ++it){
			out << *it << "</br>";
		}
		out << "</body></html>";
		
		return true;
	}
	
	bool response(){
		FCGIContext context(environment());

		string serviceRequest;
		string service;
		string operation;
		Fastcgipp::Http::Environment<char>::Gets g = environment().gets;
		{
			std::map< std::string, std::string >::iterator it = g.find("service");
			if (it != g.end()) {
				service = it->second;
			} else {
				cerr << "service not found" << std::endl;
				return false;
			}
			it = g.find("operation");
			if (it != g.end()) {
				operation = it->second;
			} else {
				cerr << "operation not found" << std::endl;
				for (it = g.begin(); it != g.end(); ++it) {
					cerr << it->first << "=" << it->second << std::endl;
				}
				return false;
			}
		}
		
		cerr << "Requested: "<< service << "::" << operation << endl;
		
		if(!environment().posts.empty()){
			for(Http::Environment<char>::Posts::const_iterator it=environment().posts.begin(); it!=environment().posts.end(); ++it){
				cerr << "Posted form data {" << it->first << "}"  << endl;
				if(it->second.type== Http::Post<char>::form){
					serviceRequest.append(it->second.value);
					cerr << "Posted a form {" << it->second.value << "}" << endl;
				}else{
					cerr << "Posted data {" << it->first << string(it->second.data(), it->second.size()) << "}" << endl;
					serviceRequest.append(string(it->second.data(), it->second.size()));
				}
			}
		}
		//If GET assume it's a ResponseOnly operation
		
		Response response;
		try{
			cerr << "try request [" << serviceRequest << "]" << endl;
			Operation& oper_ref = OperationStore::getInstance().getOperation(service, operation);
			response = oper_ref.invoke(serviceRequest);
		}catch(answer::WebMethodException &ex){
			out << "Content-Type: text/plain\r\n";
			out << "\r\n";
			out << ex.what();
			cerr << "WebException: " << ex.what();
		}catch(exception &ex){
			cerr << "Exception: "<< ex.what();
		}
		
		//TODO: when context if refactores reinstate the Codecs, for now XML conly
		const TransportInfo &transport_info = answer::Context::getInstance().transportInfo();
		
		if (environment().gets.count("debug"))
		{
			debug(serviceRequest, response.response, context);
		}else{
			out << "Content-Type: " << response.acceptType << "\r\n";
			out << "\r\n";
			out << response.response;
		}
		return true;
	}
};

} //fcgi
} //adapter
} //answer

void dlOpen(const char * path, int mode = RTLD_LAZY){
	void * handle = dlopen(path, mode);
	if (!handle) {
		throw runtime_error(dlerror());
	}
}

int main()
{
	using namespace boost::filesystem;
	
	try
	{
		char *modulesDir = getenv("modulesDir");
		char *servicesDir = getenv("servicesDir");

		//Load modules
		if (modulesDir){
			directory_iterator end_itr;
			for ( directory_iterator itr( modulesDir );
				itr != end_itr;
				++itr )
			{
				if ( extension(itr->path()) == ".so"){
					cerr << "Loading module: "<< itr->path() << endl;
					dlOpen(itr->path().c_str());
				}
			}
		}
		
		//Services
		if (servicesDir){
			directory_iterator end_itr;
			for ( directory_iterator itr( servicesDir );
				itr != end_itr;
				++itr )
			{
				if ( extension(itr->path()) == ".so"){
					cerr << "Loading service: "<< itr->path() << endl;
					dlOpen(itr->path().c_str());
				}
			}
		}

		Fastcgipp::Manager<answer::adapter::fcgi::FcgiAdapter> fcgi;
		fcgi.handler();
	}
	catch(exception& e)
	{
		cerr << e.what() << endl;
	}
}
