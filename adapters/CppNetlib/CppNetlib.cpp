#include <boost/network/protocol/http/server.hpp>
#include <boost/network/uri.hpp>
#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <vector>

// using namespace boost::network;
using namespace boost::program_options;
using namespace std;

struct handler;
typedef boost::network::http::server<handler> http_server;

struct handler {
	
	void operator() (const http_server::request &request,
										http_server::response &response) {
// 		request
// 		map<string,string> params;
// 		boost::network::uri::query_map(request.destination, params);
// 		boost::network::uri::query<query_map>(request.destination);
//     boost::network::uri::query_directive components = boost::network::uri::query(request.destination);
		
// 		cerr << query << endl;
		for (const auto &test: request.headers){
			cerr << test.name << " - " << test.value<< endl;
		}
			response = http_server::response::stock_reply(
					http_server::response::ok, "Hello, world!");
	}

	void log(http_server::string_type const &info) {
			cerr << "ERROR: " << info << '\n';
	}
private:
};

int main(int argc, char * argv[]) {
	string address;
	string port;
	
	options_description desc("Allowed options");
	desc.add_options()
    ("help", "Show this help message")
    ("address", value(&address)->default_value("0.0.0.0"), "Server Address")
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
	
	handler handler_;
	http_server::options options(handler_);
	http_server server(
			options.address(address)
							.port(port));
	server.run();
}