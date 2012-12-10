#ifndef _MOD_WPS_HH
#define _MOD_WPS_HH

#include <set>
#include <string>

#include <httpd.h>
#include <http_config.h>
#include <http_protocol.h>
#include <ap_config.h>
#include <http_request.h>
#include <http_log.h>

#include <apr_strings.h>
#include <apr_hash.h>
#include <apr_pools.h>
#include <apr_fnmatch.h>

#include <apr_tables.h>

#include <apreq2/apreq_module_apache2.h>

#include <apreq.h>
#include <apreq_module.h>
#include <apreq_param.h>
#include <apreq_cookie.h>
#include <apreq_parser.h>
#include <apreq_error.h>
#include <apreq_util.h>
#include <apr_uri.h>

#define VERSION "0.1"
#define LOGNAME "[mod_wps]"

/* Configuration structure populated by apache2.conf */
// typedef struct answer_config_rec
// {
// 
// } answer_config_rec_t;

// Configuration struct for directory or locations with MPSSession
typedef struct  {
// 	char *directory;
	bool axisRequestFormat;
// 	const char *cookie_name;
// 	const char *cookie_header;
// 	const char *uri_location;
// 	const char *uri_prelocation;
// 	const char *forbidden_location;
// 	const char *unauthorized_location;
// 	std::set<std::string> *session_allow_always;
} answer_conf_t;


extern "C" module AP_MODULE_DECLARE_DATA answer_module;

#endif // _MOD_WPS_HH