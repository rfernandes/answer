#ifndef _MONGOOSE_CONTEXT_HH_
#define _MONGOOSE_CONTEXT_HH_

#include "answer/Context.hh"
#include <mongoose.h>

namespace answer{
namespace adapter{
namespace mongoose{

class MongooseContext: public answer::Context{
  OperationInfo _operation;

public:
  MongooseContext(const mg_connection &conn);
  ~MongooseContext();

  virtual OperationInfo &operationInfo();
};

} //mongoose
} //adapter
} //answer

#endif // _MONGOOSE_CONTEXT_HH_