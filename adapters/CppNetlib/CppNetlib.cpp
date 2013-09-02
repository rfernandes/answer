#include <boost/network/protocol/http/server.hpp>
#include <boost/network/uri.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <dlfcn.h>

#include <string>
#include <iostream>
#include <vector>

#include "answer/Operation.hh"
#include "answer/Context.hh"

// using namespace boost::network;
using namespace boost::program_options;
using namespace boost::filesystem;
using namespace answer;
using namespace std;

struct CppNetLib;
typedef boost::network::http::server<CppNetLib> http_server;

class CppNetLibContext: public Context, OperationInfo, TransportInfo{
	list<string> _accepts;
	string _service;
	string _operation;
	string _rawRequest;
	
public:
	
	CppNetLibContext(const http_server::request &request){
		for (const auto &test: request.headers){
			if (test.name == "Accept"){
				boost::split(_accepts, test.value, boost::is_any_of(","), boost::token_compress_on);
				// remove "quality"
				for (list<string>::iterator itr = _accepts.begin(); itr != _accepts.end(); ++itr) {
					size_t pos = itr->find(";");
					if (pos != string::npos)
						*itr = itr->substr(0,pos);
				}
			}
		}
		vector<string> query;
// 		boost::split(query, request.destination, boost::is_any_of("&?="));
		//TODO: replace with proper cppnetlib query parsing
		boost::split(query, request.destination, boost::is_any_of("/"), boost::token_compress_on);
		if (query.size() == 3){
			_service = query[1];
			_operation = query[2];
		}
		_rawRequest = request.body;
	}

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
		return *this;
	}
	virtual TransportInfo& transportInfo(){
		return *this;
	}
	
	virtual const std::list< std::string >& accepts() const{ return _accepts; }
	virtual void addHeader(const std::string& key, const std::string& value = "", bool replace = true){
		throw std::runtime_error("addHeader unimplemented");
	}
	virtual const std::string& redirect() const{
		throw std::runtime_error("addHeader unimplemented");
	}
	virtual const std::string& redirect(const std::string& uri){
		throw std::runtime_error("addHeader unimplemented");
	}
	virtual bool redirectSet() const{
		throw std::runtime_error("addHeader unimplemented");
	}
	
	virtual const std::string& service() const{
		return _service;
	}
	virtual const std::string& operation() const{
		return _operation;
	}
	virtual const std::string& url() const{
		throw std::runtime_error("url unimplemented");
	}
	const string& rawRequest() const{
		return _rawRequest;
	}
};

class CppNetLib {
	
	void dlOpen(const char * path, int mode = RTLD_LAZY){
		void * handle = dlopen(path, mode);
		if (!handle) {
			throw runtime_error(dlerror());
		}
	}
public:

	CppNetLib(const std::string& servicesDir, const std::string& modulesDir){

		//Load modules
		if (!modulesDir.empty()){
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
		if (!servicesDir.empty()){
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
	}
	
	
	void operator() (const http_server::request &request,
										http_server::response &response) {
		try{
			CppNetLibContext context(request);
			Operation& oper_ref = OperationStore::Instance().operation(context.service(), context.operation());

			Response serviceResponse = oper_ref.invoke(context.rawRequest());
			
			response = http_server::response::stock_reply(
					http_server::response::ok, serviceResponse.response);
		}catch (std::exception& ex){
			cerr << ex.what() << endl;
		}
	}

	void log(http_server::string_type const &info) {
			cerr << "ERROR: " << info << '\n';
	}
};

int main(int argc, char * argv[]) {
	string address;
	string port;
	string servicesDir;
	string modulesDir;
	
	options_description desc("Allowed options");
	desc.add_options()
    ("help", "Show this help message")
    ("address", value(&address)->default_value("0.0.0.0"), "Server Address")
    ("servicesDir", value(&servicesDir)->required(), "Server Address")
    ("modulesDir", value(&modulesDir), "Server Address")
    ("port", value(&port)->default_value("8000"), "Server Port")
	;
	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	if (vm.count("help")){
		cout << desc << endl;
		return 0;
	}
	
// 	store(parse_config_file("answer.conf", desc), vm);
	notify(vm);
	
	CppNetLib handler_(servicesDir, modulesDir);
	http_server::options options(handler_);
	http_server server(
			options.address(address)
							.port(port));
	server.run();
}