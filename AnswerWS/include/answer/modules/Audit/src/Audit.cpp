#include "anubis/webservice/webmodule/Audit.hh"

using namespace std;
using namespace answer;

#include <iostream>

namespace anubis {
namespace webservice {
namespace webmodule{

	Audit::Audit(){
	}

	answer::WebModule::FlowStatus Audit::inFlow(InFlowContext &context){
		cerr << "AUDIT: Invoking "<< context.getOperation().getName() << endl;
		return OK;
	}

	answer::WebModule::FlowStatus Audit::outFlow(OutFlowContext &context){
		cerr << "AUDIT: Done Invoking "<< context.getOperation().getName() << endl;
		return OK;
	}

	answer::WebModule::FlowStatus Audit::outFlowFault(OutFlowContext &context){
		cerr << "AUDIT: Error Invoking "<< context.getOperation().getName() << endl;
		return OK;
	}

	static RegisterWebModule<Audit> _register("Audit");

}
}
}
