#ifndef _OPERATION_HH_
#define _OPERATION_HH_

#include <type_traits>
#include <sstream>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <boost/algorithm/string.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/result_type.hpp>

#include "archive/ws_xml_iarchive.hpp"
#include "Macros.hh"
#include "Exception.hh"
#include "Codec.hh"
#include "Context.hh"
#include "Instantiation.hh"

namespace answer
{

class Operation
{
protected:
  std::string _name;
  virtual void process(Response&, const std::string &params, const Context::Accepts &accepts) = 0;
public:
  Operation(const std::string &name)
  {
    size_t pos = name.rfind("::");
    _name = (pos != name.npos ? name.substr(pos + 2) : name);
  }
  virtual ~Operation() {};

  // The invocation wrapper
  Response invoke(const std::string &params, const Context::Accepts &accepts);
};

template <typename RequestT>
RequestT requestPart(const std::string &name, const std::string &params)
{
  RequestT request;
  std::istringstream ssIn(params);
  {
    archive::ws_xml_iarchive inA(ssIn);
    inA >> boost::serialization::make_nvp(name.c_str(), request);
  }

  return request;
}

template <typename ResponseT>
void responsePart(Response &ret, const ResponseT &response, const std::string &operationName, const Context::Accepts &accepts)
{
  std::ostringstream encodedReponse;

  // Use specific codec if available
  for (const auto & accept : accepts)
  {
    if (codec::Codec(encodedReponse, accept, response))
    {
      if (accept != "*/*")   // Otherwise the Codec should set the contentType via the context
      {
        ret.contentType(accept);
      }
      break;
    }
  }

  // Use generic codec
  if (ret.contentType().empty())
  for (const auto & accept : accepts)
  {
    if (codec::GenericEncoder(encodedReponse, accept, operationName, response))
    {
      ret.contentType(accept);
      break;
    }
  }

  if (ret.contentType().empty())
    throw std::runtime_error("No appropriate codec available");

  // TODO: Rework this interface
  //  If void is overwritten and returns false, use the default
  //  implementation (Assumes no char template is defined)
  std::ostringstream wrappedReponse;
  if (!codec::ResponseWrapper<void>(wrappedReponse, encodedReponse.str(), ret.contentType(), nullptr))
  {
    codec::ResponseWrapper<char>(wrappedReponse, encodedReponse.str(), ret.contentType(), nullptr);
  }

  ret.body(wrappedReponse.str());
}

//Empty responsePart
void responsePart(Response &ret, const std::string &operationName, const Context::Accepts &accepts);

//The default template is request / response
template <typename Type, typename OperationType, typename RequestT, typename ResponseT, class Strategy  >
class OperationHandler: public Operation
{
  Strategy _methodHandle;
  OperationType _op;
public:
  OperationHandler(OperationType op, const std::string &name): Operation(name), _op(op) {}

protected:
  void process(Response &ret, const std::string &params, const Context::Accepts &accepts) override
  {
    RequestT request = requestPart<RequestT>(_name, params);
    Type &type(_methodHandle.Instance());
    ResponseT response((type.*_op)(request));
    responsePart(ret, response, _name, accepts);
  }
};

//Partial specialization for Response only
template <typename Type, typename OperationType, typename ResponseT, class Strategy  >
class OperationHandler<Type, OperationType, boost::mpl::void_, ResponseT, Strategy>: public Operation
{
  Strategy _methodHandle;
  OperationType _op;
public:
  OperationHandler(OperationType op, const std::string &name): Operation(name), _op(op) {}

protected:
  void process(Response &ret, const std::string &, const Context::Accepts &accepts) override
  {
    Type &type(_methodHandle.Instance());
    ResponseT response((type.*_op)());
    responsePart(ret, response, _name, accepts);
  }
};

//Partial specialization for Request only
template <typename Type, typename OperationType, typename RequestT, class Strategy  >
class OperationHandler<Type, OperationType, RequestT, void, Strategy>: public Operation
{
  Strategy _methodHandle;
  OperationType _op;
public:
  OperationHandler(OperationType op, const std::string &name): Operation(name), _op(op) {}

protected:
  void process(Response &ret, const std::string &params, const Context::Accepts &accepts) override
  {
    RequestT request = requestPart<RequestT>(_name, params);
    Type &type(_methodHandle.Instance());
    (type.*_op)(request);
    //TODO: Add empty return concept (JSON needs null or {}) xml needs empty node
    responsePart(ret, _name, accepts);
  }
};

namespace detail
{

// Declaration of general template
template<typename Pmf> struct class_;
// Partial specialisation for pointers to
// member functions

//Request response
template < typename Result, typename Class,
         typename Arg >
struct class_<Result(Class:: *)(Arg)>
{
  typedef Class type;
};
//Request response const
template < typename Result, typename Class,
         typename Arg >
struct class_<Result(Class:: *)(Arg) const>
{
  typedef Class type;
};

//Response
template<typename Result, typename Class>
struct class_<Result(Class:: *)()>
{
  typedef Class type;
};
//Response const
template<typename Result, typename Class>
struct class_<Result(Class:: *)() const>
{
  typedef Class type;
};

//Request
template<typename Class, typename Arg>
struct class_<void (Class:: *)(Arg)>
{
  typedef Class type;
};
//Request const
template<typename Class, typename Arg>
struct class_<void (Class:: *)(Arg) const>
{
  typedef Class type;
};

}// namespace detail

class OperationStore
{
  std::map<std::string, std::unique_ptr<Operation>> _map;
  OperationStore();

public:

  static OperationStore &Instance();

  void registerOperation(const std::string &serviceName, const std::string &operationName, std::unique_ptr< Operation > webMethodHandle);
  Operation &operation(const std::string &serviceName, const std::string &operationName) const;

  std::vector<std::string> operationList();
};

template <typename Operation>
class RegisterOperation
{
public:
  RegisterOperation(const std::string &serviceName, const std::string &operationName, const Operation &op)
  {
    typedef typename detail::class_<Operation>::type Type;

    typedef typename boost::function_types::result_type<Operation>::type
    response;

    typedef typename boost::mpl::at_c<boost::function_types::parameter_types<Operation>, 1>::type response_type;

    using request = typename std::decay<response_type>::type;
    
    typedef OperationHandler<Type, Operation, request, response, instantiation::InstantiationStrategy<Type>> Handler;

    try
    {
      OperationStore::Instance() .registerOperation(serviceName, operationName, std::unique_ptr<Handler>(new Handler(op, operationName)));
    }
    catch (std::exception &ex)
    {
      std::cerr << "Error initializing operation [" << serviceName << "::" << operationName << "] : " << ex.what() << std::endl;
    }
  }

};

}

#define ANSWER_REGISTER(ServiceOperation, ServiceName) \
namespace {\
answer::RegisterOperation<BOOST_TYPEOF(&ServiceOperation)> ANSWER_MAKE_UNIQUE(_registrator_)(ServiceName, #ServiceOperation, &ServiceOperation);\
}

#define ANSWER_REGISTER_AS(ServiceOperation, OperationName, ServiceName) \
namespace {\
answer::RegisterOperation<BOOST_TYPEOF(&ServiceOperation)> ANSWER_MAKE_UNIQUE(_registrator_)(ServiceName, OperationName, &ServiceOperation);\
}

#endif //_OPERATION_HH_
