#include <string>
#include <dlfcn.h>
#include <stdexcept>

#include <MongooseContext.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>

#include "answer/Operation.hh"
#include "answer/Module.hh"

using namespace std;
using namespace answer;
using namespace boost::filesystem;
using namespace boost::program_options;

static void dlOpen(const std::string &path, int mode = RTLD_LAZY) {
  void *handle = dlopen(path.c_str(), mode);
  if (!handle)
  {
    throw runtime_error(dlerror());
  }
}

namespace {
  std::string baseUri;
}

static int event_handler(struct mg_connection *conn, enum mg_event ev)
{
  switch (ev) {
    case MG_AUTH:
      return MG_TRUE;   // Authorize all requests
    case MG_REQUEST:
    {
      try
      {
        if (!boost::starts_with(string(conn->uri), baseUri))
        {
          return MG_FALSE;
        }
        
        // Initialize context
        answer::adapter::mongoose::MongooseContext context(*conn);

        Operation &oper_ref = OperationStore::Instance().operation(context.operationInfo().service(), context.operationInfo().operation());
        
        std::string params;
        // Check for POST
        if (conn->content && conn->content_len)
        {
          // Assume form-encoded
          boost::property_tree::ptree pt;

          string name, value;
          string post(conn->content, conn->content_len);
          for (auto c: post)
          {
            switch (c)
            {
              case '=':
                name = value;
                value.clear();
                break;
              case '&':
                pt.put(context.operationInfo().operation() + "." + name, value);
                value.clear();
                name.clear();
                break;
              default:
                value.push_back(c);
                break;
            }
          }
          pt.put(context.operationInfo().operation() + "." + name, value);

          stringstream ss;
          boost::property_tree::write_xml(ss, pt);
          params = ss.str().substr(ss.str().find_first_of("\n") + 1);
          cerr << "Params "<< params << endl;
        }

        //Doing context.response(response) would overwrite other data such as status headers and cookies
        //TODO: Perhaps invoke should return a ProtoResponse or take Reponse as a parameter.
        Response response = oper_ref.invoke(params, context.accepts());
        mg_send_header(conn, "Content-Type", response.contentType().c_str());
        mg_printf_data(conn, "%s", response.body().c_str());
      }
      catch (answer::WebMethodException &ex)
      {
        mg_send_status(conn, 302);
        mg_send_header(conn, "Content-Type", "text/plain");
        mg_printf_data(conn, "%s", ex.what());
      }
      catch (answer::ModuleException &ex)
      {
        mg_send_status(conn, 302);
        mg_send_header(conn, "Content-Type", "text/plain");
        mg_printf_data(conn, "%s", ex.what());
        cerr << "ModuleException: " << ex.what();
      }
      catch (exception &ex)
      {
        mg_send_status(conn, 302);
        mg_send_header(conn, "Content-Type", "text/plain");
        cerr << "Exception: " << ex.what();
      }
      return MG_TRUE;   // Mark as processed
    }
    default:
      break;
  }
  return MG_FALSE;  // Rest of the events are not processed
}

int main(int argc, char *argv[]) {

  unsigned port;
  std::string mongoosePath;
  vector<string> servicesDir;
  
  options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "Display help")
      ("port,p", value(&port)->default_value(8080), "Http port")
      ("root,r", value(&mongoosePath)->implicit_value("."), "Path to document root")
      ("services,s", value(&servicesDir), "Services library directories")
      ("base,b", value(&baseUri)->default_value("/services"), "Services base directory")
  ;

  if (argc == 1) {
    cout << desc << endl;
    return 0;
  }
  
  positional_options_description p;
  p.add("services", -1);

  variables_map vm;
  store(command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  notify(vm);
  
  struct mg_server *server = mg_create_server(NULL, event_handler);
  if (!mongoosePath.empty())
    mg_set_option(server, "document_root", mongoosePath.c_str());
  mg_set_option(server, "listening_port", to_string(port).c_str());
  
  //Load services
  for (const auto &servicePath: servicesDir) {
    directory_iterator end_itr;
    for (directory_iterator itr(servicePath); itr != end_itr; ++itr){
      if (extension(itr->path()) == ".so")
      {
        cout << "Loading " << itr->path() << endl;
        dlOpen(itr->path().string(), RTLD_GLOBAL | RTLD_NOW);
      }
    }
  }

  for (;;) {
    mg_poll_server(server, 1000);  // Infinite loop, Ctrl-C to stop
  }
  mg_destroy_server(&server);

  return 0;
}
