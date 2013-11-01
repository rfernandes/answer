#include <iostream>
#include <chrono>
#include <answer/Module.hh>

class LogModule: public answer::Module{
	std::ofstream _log;
	std::chrono::time_point<std::chrono::system_clock> _start;

public:
	LogModule():_log("/tmp/logExample"){
	}
	
	virtual FlowStatus inFlow ( answer::Context& context ){
		log << "Requesting: "
				<< context.operationInfo().service()
				<< "/" << context.operationInfo().operation() << std::endl;
		_start = std::chrono::system_clock::now();
		return OK;
	}
	virtual FlowStatus outFlow ( answer::Context& context ){
		std::time_t end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-_start;
		log << "Requesting[OK]: "
				<< context.operationInfo().service()
				<< "/" << context.operationInfo().operation()
				<< " took: " << elapsed_seconds.count()
				<< std::endl;
		
		return OK;
	}
	virtual FlowStatus outFlowFault ( answer::Context& context ){
		std::time_t end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-_start;
		log << "Requesting[FAULT]: "
				<< context.operationInfo().service()
				<< "/" << context.operationInfo().operation()
				<< " took: " << elapsed_seconds.count()
				<< std::endl;
		return OK;
	}
};

ANSWER_REGISTER_MODULE(LogModule)