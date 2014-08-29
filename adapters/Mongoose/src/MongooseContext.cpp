#include "MongooseContext.h"

using namespace std;

namespace answer{
namespace adapter{
namespace mongoose{

MongooseContext::MongooseContext(const mg_connection &conn)
: _operation(conn.uri)
{
  // TODO: make this case insensitive
  const auto acceptsHeader = mg_get_header(&conn, "Accept");
  if (acceptsHeader)
  {
    cerr << acceptsHeader << endl;
    accepts(acceptsHeader);
  }
}

MongooseContext::~MongooseContext()
{
}
OperationInfo &MongooseContext::operationInfo()
{
  return _operation;
}



} //mongoose
} //adapter
} //answer
