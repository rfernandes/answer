#include "answer/OperationStore.hh"
#include "answer/WebModule.hh"
#include "answer/Context.hh"

#include "ApacheModule.hh"

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <dlfcn.h>

using namespace boost::algorithm;
using namespace answer;
using namespace std;

string xmlFromAxisXml(const string &service, const string &operation, const string& parameters){

	string file("/usr/axis2c/services/");
	file.append(service).append("/services.xml");
	
	xmlpp::DomParser parser;
	parser.parse_file(file);

// 		cerr << xmlpp::Element*(parser.get_document()->get_root_node()->find("//operation/parameter").front())->get_child_text()->get_content;

	xmlpp::NodeSet nodeSet = parser.get_document()->get_root_node()->find("//operation[@name='"+operation+"']/parameter[@name='RESTLocation']");
	
	cerr << nodeSet.size() << endl;
	
	xmlpp::Element* location = static_cast<xmlpp::Element*>(nodeSet.front());
	string locationString = location->get_child_text()->get_content();
	
	stringstream xml;
	
//     string str1("hello abc-*-ABC-*-aBc goodbye");

	typedef list< string > split_list_type;
	typedef vector< string > split_vector_type;
	
	split_list_type paramsName; // #2: Search for tokens
	split(paramsName, locationString, is_any_of("/{}"), token_compress_on ); // SplitVec == { "hello abc","ABC","aBc goodbye" }
	
	paramsName.pop_front();
	paramsName.pop_back();
	
	split_vector_type params; // #2: Search for tokens
	split(params, parameters, is_any_of("/"), token_compress_on ); // SplitVec == { "hello abc","ABC","aBc goodbye" }
	
	int idx = 0;
	xml << "<" << operation << ">";
	for (list< string >::const_iterator it = paramsName.begin(); it != paramsName.end(); ++it, ++idx){
		xml << "<" << *it << ">" << params[idx] << "</" << *it << ">\n";
	}
	xml << "</" << operation << ">";
	cerr << "Request: ["<< xml.str() << "]" << endl;
	
	return xml.str();
}

extern "C" module AP_MODULE_DECLARE_DATA anubiswebservices_module;

static void debugRequest(const char *message, request_rec* r){
	ap_set_content_type(r, "text/html;charset=ascii") ;
	ap_rputs("<html><head><title>No anubiswebservices!</title></head>\n"
		"<body><pre>\n", r);
	ap_rputs(r->uri, r);
	ap_rputs("\n", r);
	ap_rputs(message, r);
	ap_rputs("\n", r);
	ap_rputs("</pre></body></html>\n", r);
}

class Request{
	string _service;
	string _operation;
	string _params;
	string _serviceRequest;
	
	string _body;
	
	void queryRequestFormat(apreq_handle_t* apr){
		//Alternate functions for apreq_args_get
		// apr_table_t *     apreq_params (apreq_handle_t *req, apr_pool_t *p)
		// apreq_param_t *     apreq_param (apreq_handle_t *req, const char *key)
		apreq_param_t * serviceParam = NULL;
		serviceParam = apreq_args_get(apr, "service");
		apreq_param_t * operationParam = NULL;
		operationParam = apreq_args_get(apr, "operation");
		
		if (!serviceParam || !operationParam){
			//TODO: error log incorrect params
			throw runtime_error("Missing required parameters for service or operation");
		}
		_service = serviceParam->v.data;
		_operation = operationParam->v.data;
		
		apreq_param_t * paramsParam = NULL;
		paramsParam = apreq_args_get(apr, "params");
		if (paramsParam){
			_params = paramsParam->v.data;
		}
	}
	
	void axisRequestFormat(request_rec* r){
		string uri(r->uri);
		typedef vector< string > split_vector_type;
		split_vector_type splitVec;
		split(splitVec, uri, is_any_of("/"), token_compress_on);
		if (splitVec.size() < 4){
			throw runtime_error("Invalid request format");
		}
		//TODO: for now we are ignoring 0 and 1, assuming that the base path is just one dir, this will have to me matched with base 
		//  configuration (could be /services/foo/bar, not just /services)
		_service = splitVec[2];
		_operation = splitVec[3];
		_params = uri.substr(
				splitVec[0].size() + 1 +
				splitVec[1].size() + 1 +
				splitVec[2].size() + 1 +
				splitVec[3].size() + 1);
// 		cout << "Flag is :" << _service << ' ' << _operation << ' ' << _params  << endl;
	}
	
	
// 	void fillAcceptList(request_rec* r){
// 		const char * accepts = apr_table_get(r->headers_in, "Accept");
// 		std::list<std::string> aux;
// 		boost::split(aux, accepts, boost::is_any_of(","), boost::token_compress_on);
// 		// remove "quality"
// 		for (list< string >::iterator itr = aux.begin(); itr != aux.end(); ++itr) {
// 			std::size_t pos = itr->find(";");
// 			if (pos != string::npos)
// 				*itr = itr->substr(0,pos);
// 		}
// 		answer::Context::getInstance().request().setAcceptList(aux);
// 	}
	
// 	static int bodyEntryCallback(void *rec, const char *key, const char *value){
// 		Request * that = static_cast<Request*>(rec);
// 		that->_body.append(key).append(value);
// 		cerr << "POST k[" << key << "] v[" << value << "]" << endl;
// 	}
	
public:

	Request(request_rec* r, const anubisws_conf_t& conf){
// 		apr_table_t *params_table =  NULL;
		apreq_handle_t* apreq_handle =  NULL;
		apreq_handle = apreq_handle_apache2(r);
		
		if (conf.axisRequestFormat){
			axisRequestFormat(r);
		}else{
			queryRequestFormat(apreq_handle);
		}
		
// 		answer::Context::getInstance().request().reset();
// 		fillAcceptList(r);
		
		switch (r->method_number){
			case M_GET:
				_serviceRequest = xmlFromAxisXml(_service, _operation, _params);
				break;
			case M_POST:
				cerr << "Handling POST:" << _operation << endl;
				ap_setup_client_block(r, REQUEST_CHUNKED_DECHUNK);
				if ( ap_should_client_block(r) ) {
					char buffer[1024];
					int len;
					while ( (len = ap_get_client_block(r, buffer, 1024)) > 0 ) {
						_body.append(buffer, len);
					}
				}
// TODO: This is usefull for post data in multipart/form-data or application/x-www-form-urlencoded
// 				{
// 					const apr_table_t *body_args = apr_table_make(r->pool, 1);
// 					apreq_body(apreq_handle, &body_args);
// 					if (!apr_is_empty_table(body_args)){
// 						cerr << "Processing body POST:" << _serviceRequest << endl;
// 						apr_table_do(bodyEntryCallback, this, body_args, NULL);
// 					}
// 				}
				_serviceRequest = _body;
				cerr << "Handling POST : [" << _serviceRequest << "]" << endl;
				break;
		}
	}
	
	const string& getService() const{ return _service; }
	const string& getOperation() const{ return _operation; }
	const string& getParams() const{ return _params; }
	const string& getServiceRequest() const{ return _serviceRequest; }
	
};

static int anubiswebservices_handler(request_rec* r) {
//     int anubiswebservicesrand = (int) (rand());
	if (!r->handler || strcmp(r->handler, "anubiswebservices") ) {
		return DECLINED;
	}
	anubisws_conf_t *conf = static_cast<anubisws_conf_t *>(ap_get_module_config(r->server->module_config, &anubiswebservices_module));
	if (!conf){
		return DECLINED;
	}

	try{
		
		Request req(r, *conf);
		
		Operation& oper_ref = OperationStore::getInstance().getOperation(req.getOperation());
		

			//TODO: when context if refactores reinstate the Codecs, for now XML conly
// 		ResponseContext& response_context = answer::Context::getInstance().response();
// 		response_context.reset();
		string serviceResponse =  oper_ref.invoke(req.getServiceRequest());
		
// 		ap_set_content_type(r, response_context.getContentType().c_str());
// 		for (list< pair< string, string > >::const_iterator it = response_context.getAdditionalHeaders().begin(); it != response_context.getAdditionalHeaders().end(); ++it) {
// 			apr_table_add (r->headers_out, it->first.c_str(), it->second.c_str());
// 		}
// 		// if it's a location response, return code must change to 302
// 		if ( response_context.isLocationResponse() ) {
// 			r->status = HTTP_MOVED_TEMPORARILY;
// 		}

// 		ap_set_content_type(r, "text/xml;charset=utf-8") ;
		ap_rwrite(serviceResponse.c_str(), serviceResponse.size(), r);
	}catch (std::exception & ex){
		ap_set_content_type(r, "text/html;charset=ascii") ;
		debugRequest(ex.what(), r);
	}
	return OK;
}

static void dlOpen(const char * path, int mode = RTLD_LAZY){
	void * handle = dlopen(path, mode);
	if (!handle) {
		throw runtime_error(dlerror());
	}
}

static int anubiswebservices_init_handler(apr_pool_t *p, apr_pool_t */*plog*/, apr_pool_t */*ptemp*/,server_rec */*s*/)
{
	ap_add_version_component(p, "AnubisWebservices");

	dlOpen("/opt/wps/services/libwps_auth.so");
	dlOpen("/opt/wps/services/libwps_category.so");
	dlOpen("/opt/wps/services/libwps_hierarchy.so");
	dlOpen("/opt/wps/services/libwps_data.so");
	dlOpen("/opt/wps/services/libwps_policy.so");
	dlOpen("/opt/wps/services/libwps_reports.so");
	dlOpen("/opt/wps/services/libwps_stats.so");

	return OK;
}

static void anubiswebservices_hooks(apr_pool_t* /*pool*/) {
	ap_hook_handler(anubiswebservices_handler, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_config(anubiswebservices_init_handler, NULL, NULL, APR_HOOK_MIDDLE);
}


extern "C"
{
// Creates server-wide config structures
static void * create_anubiswebservices_config( apr_pool_t * p, server_rec * /*s*/)
{
	anubisws_conf_t *conf = static_cast<anubisws_conf_t *>(apr_palloc(p, sizeof(anubisws_conf_t)));
	conf->axisRequestFormat = false;
	return conf;
}

const char *set_axis_request_format(cmd_parms *cmd, void */*cfg*/, int flag) {
	anubisws_conf_t *conf = static_cast<anubisws_conf_t *>(ap_get_module_config(cmd->server->module_config, &anubiswebservices_module));
	server_rec *svr_rec = cmd->server;
	ap_log_error(APLOG_MARK, APLOG_DEBUG, APR_SUCCESS, svr_rec, "%s AXIS REQUEST FORMAT ENABLED", LOGNAME);

	if (conf && flag)
		conf->axisRequestFormat = true;
	return NULL;
}
	
// Module commands
static const command_rec cmds[] = {
		AP_INIT_FLAG   ("AWSAxisRequestFormat", (cmd_func) set_axis_request_format, NULL, RSRC_CONF, "Recognize axis2c requests (/service/operation/param1/...)"),
// 		AP_INIT_TAKE1  ("MPSCookie",           		(cmd_func) set_cookie_name, NULL, ACCESS_CONF, "MPS Session cookie name"),
// 		AP_INIT_TAKE1  ("MPSLocation",          	(cmd_func) set_uri_location, NULL, ACCESS_CONF, "MPS base location"),
// 		AP_INIT_TAKE1  ("MPSLocationPrepend",   	(cmd_func) set_uri_prelocation, NULL, ACCESS_CONF, "MPS prepended location"),
// 		AP_INIT_TAKE1  ("MPSForbiddenLocation",      	(cmd_func) set_forbidden_location, NULL, ACCESS_CONF, "MPS Session URL location for Forbidden Responses"),
// 		AP_INIT_TAKE1  ("MPSUnauthorizedLocation",   	(cmd_func) set_unauthorized_location, NULL, ACCESS_CONF, "MPS Session URL location for Unauthorized Responses"),
// 		AP_INIT_FLAG   ("MPSEnableVSP",			(cmd_func) set_enable_vsp, NULL, ACCESS_CONF, "MPS enables VSP flag"),
// 		AP_INIT_FLAG   ("MPSEnableOutbound",		(cmd_func) set_enable_outbound, NULL, ACCESS_CONF, "MPS enables OUTBOUND flag"),
// 		AP_INIT_ITERATE("MPSSessionAllowAlways",   	(cmd_func) set_session_allow_always, NULL, ACCESS_CONF, "MPS Session allowed file types"),
		{ NULL }
};

module AP_MODULE_DECLARE_DATA anubiswebservices_module = {
	STANDARD20_MODULE_STUFF,
	NULL,                  /* create per-dir    config structures */
	NULL,                  /* merge  per-dir    config structures */
	create_anubiswebservices_config,                  /* create per-server config structures */
	NULL,                  /* merge  per-server config structures */
	cmds,                  /* table of config file commands       */
	anubiswebservices_hooks/* register hooks                      */
};

}