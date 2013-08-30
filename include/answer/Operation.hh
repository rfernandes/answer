#ifndef _OPERATION_HH_
#define _OPERATION_HH_

#include <sstream>
#include <string>
#include <stdexcept>
#include <exception>
#include <boost/algorithm/string.hpp>

#include "archive/ws_xml_oarchive.hpp"
#include "archive/ws_xml_iarchive.hpp"
#include "Exception.hh"
#include "Codec.hh"
#include "Context.hh"

namespace answer {

struct Response{
	std::string response;
	std::string acceptType;
};
	
class Operation {
protected:
	std::string _name;
public:
	Operation(const std::string& name):_name(name){}
    virtual ~Operation() {};
    //The invocation wrapper
    virtual Response invoke(const std::string&, const std::string & ="")=0;
};

template <typename RequestT>
RequestT requestPart (const std::string& name, const std::string& params, const std::string& prefix){
	RequestT request;
	
	size_t pos = name.rfind("::");
	std::string filteredName(pos != name.npos ? name.substr(pos + 2) : name );
	std::string operationName(prefix);
	if (!operationName.empty()){
		operationName.append(":");
	}
	operationName.append(filteredName);

	std::istringstream ssIn(params);
	{
		answer::archive::ws_xml_iarchive inA(ssIn);
		inA >> boost::serialization::make_nvp(operationName.c_str(), request);
	}
	
	return request;
}

template <typename ResponseT>
void responsePart ( Response& ret, const ResponseT& response){
	// TODO: we only look at the first accept. if it fails, we'll default to xml immediately
	const std::list<std::string>& accept_headers_list = answer::Context::Instance().transportInfo().accepts();
	std::list<std::string>::const_iterator it = accept_headers_list.begin();
	
	std::ostringstream encodedReponse;
	std::ostringstream wrappedReponse;
	for (; it !=accept_headers_list.end(); ++it){
		// Search for custom codecs
		if(codec::Codec(encodedReponse, *it, response)) {
			ret.acceptType = *it;
			break;
		}
	}
	// Use a generic codec
	if (it == accept_headers_list.end() )
	for (it = accept_headers_list.begin(); it !=accept_headers_list.end(); ++it){
		if (codec::GenericCodec(encodedReponse, *it, response)){
			ret.acceptType = *it;
			break;
		}
	}
	if (it == accept_headers_list.end() ){
		throw std::runtime_error("No appropriate codec available");
	}
	
	// TODO: Rework this interface
	//  If void is overwritten and returns false, use the default
	//  implementation (Assumes no char template is defined)
	if (!codec::ResponseWrapper<void>( wrappedReponse, encodedReponse.str(), ret.acceptType, NULL)){
		codec::ResponseWrapper<char>( wrappedReponse, encodedReponse.str(), ret.acceptType, NULL);
	}
	ret.response = wrappedReponse.str();
}

//The default template is request / response
template <typename Type, typename OperationType, typename RequestT, typename ResponseT, class Strategy  >
class OperationHandler: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op, const std::string &name):Operation(name), _op(op){}
	
	virtual Response invoke ( const std::string& params, const std::string& prefix){
		Response ret;
		try {
			RequestT request = requestPart<RequestT>(_name, params, prefix);
			Type &type(_methodHandle.Instance());
			ResponseT response( (type.*_op)(request) );
			responsePart(ret, response);
		} catch (const WebMethodException &ex) {
			std::ostringstream wrappedReponse;
			if (!codec::ResponseWrapper<void>(wrappedReponse, "", ret.acceptType, &ex)){
				codec::ResponseWrapper<char>(wrappedReponse, "", ret.acceptType, &ex);
			}
			ret.response = wrappedReponse.str();
		} catch (const std::exception &ex) {
			std::cerr << "Exception: " << ex.what() << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error, attempting to proceed" <<std::endl;
		}
		return ret;
	}
};

//Partial specialization for Response only
template <typename Type, typename OperationType, typename ResponseT, class Strategy  >
class OperationHandler<Type, OperationType, boost::mpl::void_, ResponseT, Strategy>: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op, const std::string &name):Operation(name), _op(op){}
public:
	virtual Response invoke ( const std::string& , const std::string&){
		Response ret;
		try {
			Type &type(_methodHandle.Instance());
			ResponseT response( (type.*_op)() );
			responsePart(ret, response);
		} catch (const WebMethodException &ex) {
			std::ostringstream wrappedReponse;
			if (!codec::ResponseWrapper<void>(wrappedReponse, "", ret.acceptType, &ex)){
				codec::ResponseWrapper<char>(wrappedReponse, "", ret.acceptType, &ex);
			}
			ret.response = wrappedReponse.str();
		} catch (const std::exception &ex) {
			std::cerr << "Exception: " << ex.what() << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error, attempting to proceed" <<std::endl;
		}
		return ret;
	}
};

//Partial specialization for Request only
template <typename Type, typename OperationType, typename RequestT, class Strategy  >
class OperationHandler<Type, OperationType, RequestT, void, Strategy>: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op, const std::string &name):Operation(name), _op(op){}

	virtual Response invoke ( const std::string& params , const std::string& prefix){
		Response ret; //Empty ret
		std::ostringstream wrappedReponse;
		try {
			RequestT request = requestPart<RequestT>(_name, params, prefix);
			Type &type(_methodHandle.Instance());
			(type.*_op)(request);
// 			codec::ResponseWrapper<void>(wrappedReponse, "", "", NULL);
		} catch (const WebMethodException &ex) {
			//TODO: Tricky case, no response expected but webexception thrown. Loggin for now
			std::cerr << "WebException: " << ex.what() << std::endl;
		} catch (const std::exception &ex) {
			std::cerr << "Exception: " << ex.what() << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error, attempting to proceed" <<std::endl;
		}
		return ret;
	}
};

}

#endif //_OPERATION_HH_
