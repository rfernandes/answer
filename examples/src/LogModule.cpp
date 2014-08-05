#include <iostream>
#include <chrono>
#include <answer/Module.hh>

using namespace std;

class LogModule: public answer::Module
{
  ofstream _log;
  chrono::time_point<chrono::system_clock> _start;

public:
  LogModule(): _log("/tmp/logExample")
  {
  }

  virtual FlowStatus inFlow(answer::Context &context)
  {
    log << "Requesting: "
        << context.operationInfo().service()
        << "/" << context.operationInfo().operation() << endl;
    _start = chrono::system_clock::now();
    return OK;
  }
  virtual FlowStatus outFlow(answer::Context &context)
  {
    time_t end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - _start;
    log << "Requesting[OK]: "
        << context.operationInfo().service()
        << "/" << context.operationInfo().operation()
        << " took: " << elapsed_seconds.count()
        << endl;

    return OK;
  }
  virtual FlowStatus outFlowFault(answer::Context &context)
  {
    time_t end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - _start;
    log << "Requesting[FAULT]: "
        << context.operationInfo().service()
        << "/" << context.operationInfo().operation()
        << " took: " << elapsed_seconds.count()
        << endl;
    return OK;
  }
};

ANSWER_REGISTER_MODULE(LogModule)
