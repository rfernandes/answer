#include "answer/OperationStore.hh"
#include "answer/WebModule.hh"
#include "answer/Context.hh"

#include "ApacheModule.hh"

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <dlfcn.h>

using namespace boost::algorithm;
using namespace answer;
using namespace std;

namespace answer{
	const char* currentService = NULL;
}

extern "C" module AP_MODULE_DECLARE_DATA answer_module;

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

	Request(request_rec* r, const answer_conf_t& conf){
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

static int answer_handler(request_rec* r) {
//     int answerrand = (int) (rand());
	if (!r->handler || strcmp(r->handler, "answer") ) {
		return DECLINED;
	}
	answer_conf_t *conf = static_cast<answer_conf_t *>(ap_get_module_config(r->server->module_config, &answer_module));
	if (!conf){
		return DECLINED;
	}

	try{
		
		Request req(r, *conf);
		
		Operation& oper_ref = OperationStore::getInstance().getOperation(req.getService(), req.getOperation());
		

     //TODO: when context if refactores reinstate the Codecs, for now XML conly
		Response serviceResponse =  oper_ref.invoke(req.getServiceRequest());
		
// 		ap_set_content_type(r, response_context.getContentType().c_str());
// 		for (list< pair< string, string > >::const_iterator it = response_context.getAdditionalHeaders().begin(); it != response_context.getAdditionalHeaders().end(); ++it) {
// 			apr_table_add (r->headers_out, it->first.c_str(), it->second.c_str());
// 		}
// 		// if it's a location response, return code must change to 302
// 		if ( response_context.isLocationResponse() ) {
// 			r->status = HTTP_MOVED_TEMPORARILY;
// 		}

// 		ap_set_content_type(r, "text/xml;charset=utf-8") ;
		ap_rwrite(serviceResponse.response.c_str(), serviceResponse.response.size(), r);
	}catch (std::exception & ex){
		ap_set_content_type(r, "text/html;charset=ascii");
	}
	return OK;
}

static void dlOpen(const char * path, int mode = RTLD_LAZY){
	void * handle = dlopen(path, mode);
	if (!handle) {
		throw runtime_error(dlerror());
	}
}

static int answer_init_handler(apr_pool_t *p, apr_pool_t */*plog*/, apr_pool_t */*ptemp*/,server_rec */*s*/)
{
	ap_add_version_component(p, "Answer");

  //TODO: Load services
  using namespace boost::filesystem;
  
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


	return OK;
}

static void answer_hooks(apr_pool_t* /*pool*/) {
	ap_hook_handler(answer_handler, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_config(answer_init_handler, NULL, NULL, APR_HOOK_MIDDLE);
}


extern "C"
{
// Creates server-wide config structures
static void * create_answer_config( apr_pool_t * p, server_rec * /*s*/)
{
	answer_conf_t *conf = static_cast<answer_conf_t *>(apr_palloc(p, sizeof(answer_conf_t)));
	conf->axisRequestFormat = false;
	return conf;
}

const char *set_axis_request_format(cmd_parms *cmd, void */*cfg*/, int flag) {
	answer_conf_t *conf = static_cast<answer_conf_t *>(ap_get_module_config(cmd->server->module_config, &answer_module));
	server_rec *svr_rec = cmd->server;
	ap_log_error(APLOG_MARK, APLOG_DEBUG, APR_SUCCESS, svr_rec, "%s AXIS REQUEST FORMAT ENABLED", LOGNAME);

	if (conf && flag)
		conf->axisRequestFormat = true;
	return NULL;
}
	
// Module commands
static const command_rec cmds[] = {
		AP_INIT_FLAG   ("AnswerAxisRequestFormat", (cmd_func) set_axis_request_format, NULL, RSRC_CONF, "Recognize axis2c requests (/service/operation/)"),
		{ NULL }
};

module AP_MODULE_DECLARE_DATA answer_module = {
	STANDARD20_MODULE_STUFF,
	NULL,                  /* create per-dir    config structures */
	NULL,                  /* merge  per-dir    config structures */
	create_answer_config,                  /* create per-server config structures */
	NULL,                  /* merge  per-server config structures */
	cmds,                  /* table of config file commands       */
	answer_hooks/* register hooks                      */
};

}