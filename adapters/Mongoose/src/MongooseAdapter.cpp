#include <mongoose.h>
#include <string>
#include <boost/filesystem.hpp>
#include <dlfcn.h>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

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

class MongooseOperationInfo: public OperationInfo
{
  std::string _operation;
  std::string _service;
  std::string _url;

public:
  MongooseOperationInfo(mg_connection *conn):
    _url(conn->uri+1) 
  {
    char buffer[256];
    mg_get_var(conn, "service", buffer, sizeof(buffer));
    _service = buffer;
    mg_get_var(conn, "operation", buffer, sizeof(buffer));
    _operation = buffer;
  }

  virtual const string &operation() const{
    return _operation;
  }
  virtual const string &service() const{
    return _service;
  }
  virtual const string &url() const{
    return _url;
  }
};

static int event_handler(struct mg_connection *conn, enum mg_event ev)
{
  switch (ev) {
    case MG_AUTH:
      return MG_TRUE;   // Authorize all requests
    case MG_REQUEST:
    {
      try
      {
        MongooseOperationInfo info(conn);
        if (!boost::starts_with(info.url(), "services/"))
        {
          return MG_FALSE;
        }

        Operation &oper_ref = OperationStore::Instance().operation(info.service(), info.operation());
        
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
                pt.put(info.operation() + "." + name, value);
                value.clear();
                name.clear();
                break;
              default:
                value.push_back(c);
                break;
            }
          }
          pt.put(info.operation() + "." + name, value);

          stringstream ss;
          boost::property_tree::write_xml(ss, pt);
          params = ss.str().substr(ss.str().find_first_of("\n") + 1);
          cerr << "Params "<< params << endl;
        }

        vector<string> accepts;
        if (mg_get_header(conn, "Accept"))
        {
          string accept(mg_get_header(conn, "Accept"));
          string mime;
          for (auto c: accept)
          {
            switch (c)
            {
              case ' ': // trim
                break;
              case ';': // ignore accept-params
                mime.clear();
                break;
              case ',':
                accepts.emplace_back(mime);
                cerr << mime << endl;
                mime.clear();
                break;
              default:
                mime.push_back(c);
                break;
            }
          }
          accepts.emplace_back(mime);
          cerr << mime << endl;
        }
        else
        {
          // XML encoder is acting up, default to app/json for now
          accepts.emplace_back("application/json");
        }

        //Doing context.response(response) would overwrite other data such as status headers and cookies
        //TODO: Perhaps invoke should return a ProtoResponse or take Reponse as a parameter.
        Response response = oper_ref.invoke(params, accepts);
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
  vector<string> servicePaths;
  
  options_description desc("Allowed options");
  desc.add_options()
      ("help", "Display help")
      ("port,p", value(&port)->default_value(8080), "Http port")
      ("services,s", value(&servicePaths), "Services path")
  ;

  if (argc == 1) {
    cout << desc << endl;
    return 0;
  }
  
  positional_options_description p;
  p.add("services", -1);

  variables_map vm;
  store(command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
  notify(vm);
  
  
  struct mg_server *server = mg_create_server(NULL, event_handler);
  mg_set_option(server, "document_root", ".");
  mg_set_option(server, "listening_port", to_string(port).c_str());
  
  //Load services
  for (const auto &servicePath: servicePaths) {
    directory_iterator end_itr;
    for (directory_iterator itr(servicePath);
        itr != end_itr;
        ++itr){
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
