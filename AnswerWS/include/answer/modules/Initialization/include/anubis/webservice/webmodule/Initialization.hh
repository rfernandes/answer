#ifndef WPSMODULEINITIALIZATION_HH
#define WPSMODULEINITIALIZATION_HH

#include <string>
#include <anubis/webservice/WebModule.hh>
#include <anubis/webservice/Params.hh>

#include <map>
#include <boost/any.hpp>


namespace anubis {
namespace webservice {
namespace webmodule {

class Initialization: public answer::WebModule {
public:
    Initialization();
    virtual FlowStatus inFlow(answer::InFlowContext &context);
    virtual FlowStatus outFlow(answer::OutFlowContext &context);
    virtual FlowStatus outFlowFault(answer::OutFlowContext &context);

    template <typename T>
    static T& getRef(const std::string& key) {
        typename std::map<std::string, boost::any>::iterator it = singletons.find(key);

        if (it == singletons.end()) {
            throw std::runtime_error("Bad singleton name");
        }

         T *ret = NULL;

        try {
            ret = boost::any_cast<T *>(it->second);
            
        } catch (boost::bad_any_cast &ex) {

            throw std::runtime_error("Bad singleton cast type");
        }
        return (*ret);
    }


private:
    static std::map<std::string, boost::any> singletons;
    static std::string _config_file;


};
}
}
}

#endif // WPSMODULEINITMULTITON
