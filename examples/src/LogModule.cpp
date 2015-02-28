#include <iostream>
#include <chrono>
#include <fstream>
#include <answer/Module.hh>
#include <answer/Operation.hh>

using namespace std;

class LogModule: public answer::Module
{
  ostream &_log;
  chrono::time_point<chrono::system_clock> _start;

public:
  LogModule(): _log(cout)
  {
    const auto operations = answer::OperationStore::Instance().operationList();
    for (auto &&operation : operations)
    {
      _log << operation << endl;
    }
  }

  virtual FlowStatus inFlow(answer::Context &context)
  {
    _log << "Requesting: "
         << context.operationInfo().service()
         << "/" << context.operationInfo().operation() << endl;
    _start = chrono::system_clock::now();
    return OK;
  }
  virtual FlowStatus outFlow(answer::Context &context)
  {
    auto end = chrono::system_clock::now();
    auto elapsed_seconds = end - _start;
    _log << "Requesting [OK]: "
         << context.operationInfo().service()
         << "/" << context.operationInfo().operation()
         << " took: " << elapsed_seconds.count()
         << endl;

    return OK;
  }
  virtual FlowStatus outFlowFault(answer::Context &context)
  {
    auto end = chrono::system_clock::now();
    auto elapsed_seconds = end - _start;
    _log << "Requesting [FAULT]: "
         << context.operationInfo().service()
         << "/" << context.operationInfo().operation()
         << " took: " << elapsed_seconds.count()
         << endl;
    return OK;
  }
};

ANSWER_REGISTER_MODULE(LogModule)
