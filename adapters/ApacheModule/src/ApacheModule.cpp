#include "answer/OperationStore.hh"
#include "answer/WebModule.hh"
#include "answer/Context.hh"

#include "ApacheModule.hh"
#include "ApacheContext.hh"

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <dlfcn.h>

using namespace boost::algorithm;
using namespace answer;
using namespace std;

namespace{
 answer_conf_t config;
}

namespace answer{
	const char* currentService = NULL;
}

extern "C" module AP_MODULE_DECLARE_DATA answer_module;

static int answer_handler(request_rec* r) {
	if (!r->handler || strcmp(r->handler, "answer_module") ) {
		return DECLINED;
	}
// 	answer_conf_t *conf = static_cast<answer_conf_t *>(ap_get_module_config(r->server->module_config, &answer_module));
// 	if (!conf){
// 		return DECLINED;
// 	}

	try{
		answer::adapter::apache::ApacheContext context(r, config);

		Operation& oper_ref = OperationStore::Instance().operation(context.operationInfo().serviceName(), context.operationInfo().operationName());

// 		Response serviceResponse =  oper_ref.invoke(context.transportInfo().redirect("REPLACE ME"));
    Response serviceResponse =  oper_ref.invoke(static_cast<answer::adapter::apache::ApacheOperationInfo &>(context.operationInfo()).getRawRequest());

// 		ap_set_content_type(r, response_context.getContentType().c_str());
// 		for (list< pair< string, string > >::const_iterator it = response_context.getAdditionalHeaders().begin(); it != response_context.getAdditionalHeaders().end(); ++it) {
// 			apr_table_add (r->headers_out, it->first.c_str(), it->second.c_str());
// 		}
// 		// if it's a location response, return code must change to 302
// 		if ( response_context.isLocationResponse() ) {
// 			r->status = HTTP_MOVED_TEMPORARILY;
// 		}
//     ap_set_content_type(r, "text/html");
//     ap_rprintf(r, "Hello, world!");
		ap_set_content_type(r, serviceResponse.acceptType.c_str()) ;
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

  //Load modules
  if (!config.modulesDir.empty()){
    directory_iterator end_itr;
    for ( directory_iterator itr( config.modulesDir );
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
  if (!config.servicesDir.empty()){
    directory_iterator end_itr;
    for ( directory_iterator itr( config.servicesDir );
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
// static void * create_answer_config( apr_pool_t * p, server_rec * /*s*/)
// {
// 	answer_conf_t *conf = static_cast<answer_conf_t *>(apr_palloc(p, sizeof(answer_conf_t)));
// 	conf->axisRequestFormat = false;
// 	return conf;
// }

const char *set_axis_request_format(cmd_parms *cmd, void */*cfg*/, int flag) {
	ap_log_error(APLOG_MARK, APLOG_DEBUG, APR_SUCCESS, cmd->server, "%s AXIS REQUEST FORMAT ENABLED", LOGNAME);

  config.axisRequestFormat = flag;
	return NULL;
}

const char *set_services_dir(cmd_parms *cmd, void */*cfg*/, char *dir) {
  config.servicesDir = dir;
  return NULL;
}

const char *set_modules_dir(cmd_parms *cmd, void */*cfg*/, char *dir) {
  config.modulesDir = dir;
  return NULL;
}
	
// Module commands
static const command_rec cmds[] = {
		AP_INIT_FLAG   ("AnswerAxisRequestFormat", (cmd_func) set_axis_request_format, NULL, RSRC_CONF, "Recognize axis2c requests (/service/operation/)"),
    AP_INIT_TAKE1  ("AnswerServicesDir", (cmd_func) set_services_dir, NULL, RSRC_CONF, "Path to modules directory"),
    AP_INIT_TAKE1  ("AnswerModulesDir", (cmd_func) set_modules_dir, NULL, RSRC_CONF, "Path to modules directory"),
		{ NULL }
};

module AP_MODULE_DECLARE_DATA answer_module = {
	STANDARD20_MODULE_STUFF,
	NULL,                  /* create per-dir    config structures */
	NULL,                  /* merge  per-dir    config structures */
 	NULL,                  /* create per-server config structures */
	NULL,                  /* merge  per-server config structures */
	cmds,                  /* table of config file commands       */
	answer_hooks/* register hooks                      */
};

}