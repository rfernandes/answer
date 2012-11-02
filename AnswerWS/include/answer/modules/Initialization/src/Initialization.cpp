#include "anubis/webservice/webmodule/Initialization.hh"

using namespace std;
using namespace answer;



#include <wps/policy/PolicyManager.hh>
#include <anubis/data/EntityDataManager.hh>

namespace anubis {
namespace webservice {
namespace webmodule {

std::string Initialization::_config_file = "/opt/wps/etc/wps.conf";


std::map<std::string, boost::any> Initialization::singletons;

Initialization::Initialization() {


    static anubis::data::EntityDataManagerSingleton em_singleton;
		static wps::policy::PolicyManagerSingleton pm_singleton;
    
		try {

        em_singleton.get_mutable_instance().readConfig(Initialization::_config_file );
        singletons["entity-manager"] = boost::any((anubis::data::EntityDataManagerSingleton *) &em_singleton);

				LOGANUBIS_INFO("WPS Entity Model Manager Initialized");

    } catch (std::exception &ex) {

				std::cerr << "INITIALIZATION ERROR: ENTITY MANAGER: " << ex.what() << std::endl;
        //TODO: EXIT OR THROW EXTREMELLY FATAL EXCEPTION
    }
    
    
    try {
			
				wps::policy::PolicyManagerScopeMap::init(Initialization::_config_file , pm_singleton);
        singletons["policy-manager"] = boost::any((wps::policy::PolicyManagerSingleton *) &pm_singleton);

				LOGANUBIS_INFO("WPS Policy Manager Initialized");
				
				
    } catch (std::exception &ex) {

				std::cerr << "INITIALIZATION ERROR: POLICY MANAGER: " << ex.what() << std::endl;
        //TODO: EXIT OR THROW EXTREMELLY FATAL EXCEPTION
    }
    



}


answer::WebModule::FlowStatus Initialization::inFlow(InFlowContext &context) {

   
    return OK;

}

answer::WebModule::FlowStatus Initialization::outFlow(OutFlowContext &context) {
    //We currently have no outFlow handling
    return OK;
}

answer::WebModule::FlowStatus Initialization::outFlowFault(OutFlowContext &context) {
    //We currently have no outFlow Fault handling
    return OK;
}

static RegisterWebModule<Initialization> _register("Initialization");

}
}
}
