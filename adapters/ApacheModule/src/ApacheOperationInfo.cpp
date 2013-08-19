#include "ApacheOperationInfo.hh"

#include <exception>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::algorithm;

namespace answer{
namespace adapter{
namespace apache{

ApacheOperationInfo::ApacheOperationInfo(request_rec* r, const answer_conf_t& conf)
{
//    apr_table_t *params_table =  NULL;
    apreq_handle_t* apreq_handle =  NULL;
    apreq_handle = apreq_handle_apache2(r);
    
    if (conf.axisRequestFormat){
      axisRequestFormat(r);
    }else{
      queryRequestFormat(apreq_handle);
    }
    
//    answer::Context::getInstance().request().reset();
//    fillAcceptList(r);
    
    switch (r->method_number){
      case M_GET:
        break;
      case M_POST:
        ap_setup_client_block(r, REQUEST_CHUNKED_DECHUNK);
        if ( ap_should_client_block(r) ) {
          char buffer[1024];
          int len;
          while ( (len = ap_get_client_block(r, buffer, 1024)) > 0 ) {
            _rawRequest.append(buffer, len);
          }
        }
// TODO: This is usefull for post data in multipart/form-data or application/x-www-form-urlencoded
//        {
//          const apr_table_t *body_args = apr_table_make(r->pool, 1);
//          apreq_body(apreq_handle, &body_args);
//          if (!apr_is_empty_table(body_args)){
//            cerr << "Processing body POST:" << _serviceRequest << endl;
//            apr_table_do(bodyEntryCallback, this, body_args, NULL);
//          }
//        }
        break;
    }
}

const std::string& ApacheOperationInfo::getOperationName() const
{
  return _operation;
}

const std::string& ApacheOperationInfo::getServiceName() const
{
  return _service;
}

const std::string& ApacheOperationInfo::getURL() const
{
  throw std::runtime_error("Get URL Unimplemented");
}

void ApacheOperationInfo::queryRequestFormat(apreq_handle_t* apr){
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
//   apreq_param_t * paramsParam = NULL;
//   paramsParam = apreq_args_get(apr, "params");
//   if (paramsParam){
//     _params = paramsParam->v.data;
//   }
}

void ApacheOperationInfo::axisRequestFormat(request_rec* r){
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
//   _params = uri.substr(
//       splitVec[0].size() + 1 +
//       splitVec[1].size() + 1 +
//       splitVec[2].size() + 1 +
//       splitVec[3].size() + 1);
}

}
}
}
