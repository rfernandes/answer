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

#include "archive/ws_xml_oarchive.hpp"
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
public:
  Operation(const std::string &name)
  {
    size_t pos = name.rfind("::");
    _name = (pos != name.npos ? name.substr(pos + 2) : name);
  }
  virtual ~Operation() {};
  //The invocation wrapper
  virtual Response invoke(const std::string &params, const std::string &prefix, const answer::Context::Accepts &accepts) = 0;
};

template <typename RequestT>
RequestT requestPart(const std::string &name, const std::string &params, const std::string &prefix)
{
  std::string operationName(prefix);
  if (!operationName.empty())
  {
    operationName.append(":");
  }
  operationName.append(name);

  RequestT request;
  std::istringstream ssIn(params);
  {
    answer::archive::ws_xml_iarchive inA(ssIn);
    inA >> boost::serialization::make_nvp(operationName.c_str(), request);
  }

  return request;
}

template <typename ResponseT>
void responsePart(Response &ret, const ResponseT &response, const std::string &operationName, const answer::Context::Accepts &accepts)
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
    if (codec::GenericCodec(encodedReponse, accept, operationName, response))
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
void responsePart(Response &ret, const std::string &operationName, const answer::Context::Accepts &accepts);

//The default template is request / response
template <typename Type, typename OperationType, typename RequestT, typename ResponseT, class Strategy  >
class OperationHandler: public Operation
{
  Strategy _methodHandle;
  OperationType _op;
public:
  OperationHandler(OperationType op, const std::string &name): Operation(name), _op(op) {}

  virtual Response invoke(const std::string &params, const std::string &prefix, const answer::Context::Accepts &accepts)
  {
    Response ret;
    try
    {
      RequestT request = requestPart<RequestT>(_name, params, prefix);
      Type &type(_methodHandle.Instance());
      ResponseT response((type.*_op)(request));
      responsePart(ret, response, _name, accepts);
    }
    catch (const WebMethodException &ex)
    {
      ret.status(Response::Status::INTERNAL_SERVER_ERROR);
      std::ostringstream wrappedReponse;
      if (!codec::ResponseWrapper<void>(wrappedReponse, "", ret.contentType(), &ex))
      {
        codec::ResponseWrapper<char>(wrappedReponse, "", ret.contentType(), &ex);
      }
      ret.body(wrappedReponse.str());
      std::cerr << "WebException: " << ex.what() << std::endl;
    }
    catch (const std::exception &ex)
    {
      ret.status(Response::Status::INTERNAL_SERVER_ERROR);
      std::cerr << "Exception: " << ex.what() << std::endl;
    }
    return ret;
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

  virtual Response invoke(const std::string & , const std::string &, const answer::Context::Accepts &accepts)
  {
    Response ret;
    try
    {
      Type &type(_methodHandle.Instance());
      ResponseT response((type.*_op)());
      responsePart(ret, response, _name, accepts);
    }
    catch (const WebMethodException &ex)
    {
      std::ostringstream wrappedReponse;
      ret.status(Response::Status::INTERNAL_SERVER_ERROR);
      if (!codec::ResponseWrapper<void>(wrappedReponse, "", ret.contentType(), &ex))
      {
        codec::ResponseWrapper<char>(wrappedReponse, "", ret.contentType(), &ex);
      }
      ret.body(wrappedReponse.str());
      std::cerr << "WebException: " << ex.what() << std::endl;
    }
    catch (const std::exception &ex)
    {
      ret.status(Response::Status::INTERNAL_SERVER_ERROR);
      std::cerr << "Exception: " << ex.what() << std::endl;
    }
    return ret;
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

  virtual Response invoke(const std::string &params , const std::string &prefix, const answer::Context::Accepts &accepts)
  {
    Response ret; //Empty ret
    std::ostringstream wrappedReponse;
    try
    {
      RequestT request = requestPart<RequestT>(_name, params, prefix);
      Type &type(_methodHandle.Instance());
      (type.*_op)(request);
      //TODO: Add empty return concept (JSON needs null or {}) xml needs empty node
      responsePart(ret, _name, accepts);
    }
    catch (const WebMethodException &ex)
    {
      ret.status(Response::Status::INTERNAL_SERVER_ERROR);
      std::cerr << "WebException: " << ex.what() << std::endl;
    }
    catch (const std::exception &ex)
    {
      ret.status(Response::Status::INTERNAL_SERVER_ERROR);
      std::cerr << "Exception: " << ex.what() << std::endl;
    }
    return ret;
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

  void registerOperation(const std::string &serviceName, const std::string &operationName, std::unique_ptr< answer::Operation > webMethodHandle);
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
    
    typedef
    OperationHandler<Type, Operation, request, response, instantiation::InstantiationStrategy<Type>>
        Handler;

    try
    {
      OperationStore::Instance()
      .registerOperation(
        serviceName,
        operationName,
        std::unique_ptr<Handler>(new Handler(op, operationName))
      );
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
