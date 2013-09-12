#include <iostream>

#include <answer/Module.hh>


class LogModule: public answer::Module{
	std::ofstream _log;
public:
	LogModule():_log("/tmp/logExample"){
	}
	
	virtual FlowStatus inFlow ( answer::Context& context ){
		log << "Requesting: " << context.operationInfo().service()
				<< std::endl
				<< "/" << context.operationInfo().operation() << std::endl;
		return OK;
	}
	virtual FlowStatus outFlow ( answer::Context& context ){
		log << "Everything ok: "<< std::endl;
		return OK;
	}
	virtual FlowStatus outFlowFault ( answer::Context& context ){
		log << "An error occured" << std::endl;
		return OK;
	}

};

ANSWER_REGISTER_MODULE(LogModule)