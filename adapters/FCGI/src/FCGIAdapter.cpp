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

class FcgiAdapter: public Fastcgipp::Request<char>
{
  bool response(){
    try{
      FCGIContext context(environment());

      ModuleStore & store = ModuleStore::Instance();
      Module::FlowStatus status = store.inFlow(context);

      //TODO: This features (skip) in unimplemented in other adapters
//       switch(status){
//         case Module::SKIP:
//         
//         break;
//         case Module::OK:
//         
//         break;
//         case Module::ERROR:
//         
//         break;
//       
//       }
      if (status != Module::SKIP){
        string serviceRequest;
        string service = context.operationInfo().service();
        string operation = context.operationInfo().operation();

        boost::property_tree::ptree pt;
        if(!environment().posts.empty()){
          for(Http::Environment<char>::Posts::const_iterator it=environment().posts.begin(); it!=environment().posts.end(); ++it){
            if(it->second.type== Http::Post<char>::form){
              pt.put(operation + "." + it->first, it->second.value);
      // //           serviceRequest.append(it->second.value);
      //          cerr << "Posted a form {" << it->second.value << "}" << endl;
      //        }else{
      //          cerr << "Posted data {" << it->first << string(it->second.data(), it->second.size()) << "}" << endl;
      //          serviceRequest.append(string(it->second.data(), it->second.size()));
            }
          }
        }
        stringstream ss;

        boost::property_tree::write_xml(ss, pt);
        // Remove the xml header, header is always present
        serviceRequest = ss.str().substr(ss.str().find_first_of("\n") + 1);
        Operation& oper_ref = OperationStore::Instance().operation(service, operation);
        //Doing context.response(response) would overwrite other data such as status headers and cookies
        //TODO: Perhaps invoke should return a ProtoResponse or take Reponse as a parameter.
        Response response = oper_ref.invoke(serviceRequest);
        context.response().body(response.body());
        if (!response.contentType().empty()){
          context.response().contentType(response.contentType());
        }
      }else{ // Read from context and return
        if (context.response().status() != Response::Status::OK_){
          out << "Status: "
              << static_cast<underlying_type<Response::Status>::type>(context.response().status())
              << ' '
              << Response::statusText.at(context.response().status()) << "\r\n";
        }
      }

      out << "Content-Type: " << context.response().contentType() << "\r\n";
      out << "Content-Length: " << context.response().body().size() << "\r\n";
      for (const auto &header: context.response().headers()){
        out << header.first << ": " << header.second << "\r\n";
      }
      out << "\r\n";
      out << context.response().body();

      if (status != Module::SKIP){
        
        //TODO: look at return status and call outFlowFault if problem
        store.outFlow(context);
        
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
      dlOpen(path.c_str(), RTLD_LAZY);
    }

    Fastcgipp::Manager<answer::adapter::fcgi::FcgiAdapter> fcgi;
    fcgi.handler();
  }
  catch(exception& e)
  {
    cerr << e.what() << endl;
  }
}
