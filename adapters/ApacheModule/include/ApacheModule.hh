#ifndef _APACHE_MODULE_HH_
#define _APACHE_MODULE_HH_

#include <set>
#include <string>

#include <httpd.h>
#include <http_protocol.h>
#include <http_log.h>

#define VERSION "0.1"
#define LOGNAME "[mod_wps]"

typedef struct
{
  std::vector<std::string> answerModulesDir;
} answer_conf_t;

extern "C" module AP_MODULE_DECLARE_DATA answer_module;

#endif // _APACHE_MODULE_HH_
