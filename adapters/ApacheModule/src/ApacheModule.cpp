#include "answer/Operation.hh"
#include "answer/Module.hh"
#include "answer/Context.hh"
#include "answer/Adapter.hh"

#include "ApacheModule.hh"
#include "ApacheContext.hh"

#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <dlfcn.h>

using namespace boost::algorithm;
using namespace answer;
using namespace std;

namespace
{
answer_conf_t config;
}

namespace answer
{
const char *currentService = NULL;
}

extern "C" module AP_MODULE_DECLARE_DATA answer_module;

static int answer_handler(request_rec *r)
{
  if (!r->handler || strcmp(r->handler, "answer_module"))
  {
    return DECLINED;
  }

  try
  {
    answer::adapter::apache::ApacheContext context(r, config);

    Operation &oper_ref = OperationStore::Instance().operation(context.operationInfo().service(), context.operationInfo().operation());

    std::string rawRequest;
    ap_setup_client_block(r, REQUEST_CHUNKED_DECHUNK);
    if (ap_should_client_block(r))
    {
      char buffer[1024];
      int len;
      while ((len = ap_get_client_block(r, buffer, 1024)) > 0)
      {
        rawRequest.append(buffer, len);
      }
    }
    
    context.response(oper_ref.invoke(rawRequest, context.accepts()));
    
//      ap_set_content_type(r, response_context.getContentType().c_str());
//      for (list< pair< string, string > >::const_iterator it = response_context.getAdditionalHeaders().begin(); it != response_context.getAdditionalHeaders().end(); ++it) {
//          apr_table_add (r->headers_out, it->first.c_str(), it->second.c_str());
//      }
//      // if it's a location response, return code must change to 302
//      if ( response_context.isLocationResponse() ) {
//          r->status = HTTP_MOVED_TEMPORARILY;
//      }
//     ap_set_content_type(r, "text/html");
//     ap_rprintf(r, "Hello, world!");
    ap_set_content_type(r, context.response().contentType().c_str()) ;
    ap_rwrite(context.response().body().c_str(), context.response().body().size(), r);
  }
  catch (std::exception &ex)
  {
    ap_set_content_type(r, "text/html;charset=ascii");
  }
  return OK;
}

static int answer_init_handler(apr_pool_t *p, apr_pool_t */*plog*/, apr_pool_t */*ptemp*/, server_rec */*s*/)
{
  ap_add_version_component(p, "Answer");

  //TODO: Load services
  using namespace boost::filesystem;

  //Load modules
  for (const auto &dir: config.answerModulesDir){
    directory_iterator end_itr;
    for (directory_iterator itr(dir);
         itr != end_itr;
         ++itr)
    {
      if (extension(itr->path()) == ".so")
      {
        dlOpen(itr->path().c_str());
      }
    }
  }

  return OK;
}

static void answer_hooks(apr_pool_t * /*pool*/)
{
  ap_hook_handler(answer_handler, NULL, NULL, APR_HOOK_MIDDLE);
  ap_hook_post_config(answer_init_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

extern "C"
{

  APLOG_USE_MODULE(answer);

  const char *set_answer_dir(cmd_parms *cmd, void */*cfg*/, char *dir)
  {
    config.answerModulesDir.push_back(dir);
    return NULL;
  }

// Module commands
  static const command_rec cmds[] =
  {
    AP_INIT_ITERATE("AnswersDir", (cmd_func) set_answer_dir, NULL, RSRC_CONF, "Path to answer modules directory"),
    { NULL }
  };

  module AP_MODULE_DECLARE_DATA answer_module =
  {
    STANDARD20_MODULE_STUFF,
    NULL,                  /* create per-dir    config structures */
    NULL,                  /* merge  per-dir    config structures */
    NULL,                  /* create per-server config structures */
    NULL,                  /* merge  per-server config structures */
    cmds,                  /* table of config file commands       */
    answer_hooks/* register hooks                      */
  };

}
