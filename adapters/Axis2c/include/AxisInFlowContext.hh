#include "answer/Module.hh"
#include "answer/Environment.hh"
#include "AxisOperation.hh"

namespace answer
{
namespace adapter
{
namespace axis
{

class AxisInFlowContext: public Context
{
  AxisOperation _operation;

public:
  AxisInFlowContext(const axutil_env_t *axis_env,
                    struct axis2_msg_ctx *msg_ctx) :
    _operation(axis_env, msg_ctx)
  {
  }

  virtual OperationInfo &operationInfo()
  {
    return _operation;
  }
};

}
}
}
