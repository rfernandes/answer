#include <fstream>
#include <algorithm>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <type_traits>

#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>
#include <dlfcn.h>

#include "FCGIContext.hh"
#include "answer/Operation.hh"
#include "answer/Module.hh"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace boost::algorithm;
using namespace std;
using namespace Fastcgipp;

namespace answer{
namespace adapter{
namespace fcgi{

class FcgiAdapter: public Fastcgipp::Request
{
  bool response(){
    try{
      FCGIContext context(environment());

      ModuleStore & store = ModuleStore::Instance();
      Module::FlowStatus status = store.inFlow(context);

      switch (status){
        // Request the webservice
        case Module::OK:{
          string requestBody;
          const string &service = context.operationInfo().service();
          const string &operation = context.operationInfo().operation();

          boost::property_tree::ptree pt;
          if(!environment().posts.empty()){
            for(const auto &env: environment().posts){
              if(env.second.type== Http::Post::form){
                pt.put(operation + "." + env.first, env.second.value);
              }
            }
          }
          stringstream ss;
          boost::property_tree::write_xml(ss, pt);
          // Remove the xml header, header is always present
          requestBody = ss.str().substr(ss.str().find_first_of("\n") + 1);
          
          Operation& oper_ref = OperationStore::Instance().operation(service, operation);
          //Doing context.response(response) would overwrite other data such as status headers and cookies
          //TODO: Perhaps invoke should return a ProtoResponse or take Reponse as a parameter.
          Response response = oper_ref.invoke(requestBody);
          //If status is the original, overwrite with the returned on (which might still be 
          if (context.response().status() == Response::Status::OK){
            context.response().status(response.status());
          }
          context.response().body(response.body());
          if (!response.contentType().empty()){
            context.response().contentType(response.contentType());
          }
        }
        break;
        case Module::ERROR:
        case Module::SKIP:
          break;
      }
      out << "Status: "
      << static_cast<underlying_type<Response::Status>::type>(context.response().status())
      << ' '
      << Response::statusText.at(context.response().status()) << "\r\n";
      out << "Content-Type: " << context.response().contentType() << "\r\n";
      out << "Content-Length: " << context.response().body().size() << "\r\n";
      for (const auto &header: context.response().headers()){
        out << header.first << ": " << header.second << "\r\n";
      }
      for (const auto &cookie: context.cookies()){
        out << "Set-Cookie: " << cookie.second << "\r\n";
      }
      
      out << "\r\n";
      out << context.response().body();

      switch (status){
        case Module::OK:
        case Module::SKIP:
          store.outFlow(context);
          break;
        case Module::ERROR:      
          store.outFlowFault(context);
          break;
      }
    }catch(answer::WebMethodException &ex){
      out << "Content-Type: text/plain\r\n";
      out << "\r\n";
      out << ex.what();
      cerr << "WebException: " << ex.what();
    }catch(answer::ModuleException &ex){
      out << "Content-Type: text/plain\r\n";
      out << "\r\n";
      out << ex.what();
      cerr << "ModuleException: " << ex.what();
    }catch(exception &ex){
      cerr << "Exception: "<< ex.what();
    }
    return true;
  }
public:
  virtual ~FcgiAdapter(){ }
};

} //fcgi
} //adapter
} //answer

void dlOpen(const char * path, int mode = RTLD_LAZY){
  void * handle = dlopen(path, mode);
  if (!handle) {
    throw runtime_error(dlerror());
  }
}

int main()
{
  using namespace boost::filesystem;
  
  try
  {
    char *modulesDir = getenv("modulesDir");
    char *servicesDir = getenv("servicesDir");

    std::set<std::string> modulePath;
    //Load modules
    if (modulesDir){
      directory_iterator end_itr;
      for ( directory_iterator itr( modulesDir );
        itr != end_itr;
        ++itr ){
        if ( extension(itr->path()) == ".so"){
          modulePath.insert(itr->path().string());
        }
      }
    }

    //Services
    if (servicesDir){
      directory_iterator end_itr;
      for ( directory_iterator itr( servicesDir );
        itr != end_itr;
        ++itr ){
        if ( extension(itr->path()) == ".so"){
          modulePath.insert(itr->path().string());
        }
      }
    }

    for(const auto& path: modulePath){
      //Symbol resolution must be GLOBAL due to implicit cast of Context
      // inherited object, otherwise called address will be wrong
      // http://gcc.gnu.org/faq.html#dso
      dlOpen(path.c_str(), RTLD_GLOBAL | RTLD_NOW);
    }

    Fastcgipp::Manager<answer::adapter::fcgi::FcgiAdapter> fcgi;
    fcgi.handler();
  }
  catch(exception& e)
  {
    cerr << e.what() << endl;
  }
}
