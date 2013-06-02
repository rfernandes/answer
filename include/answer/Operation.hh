
#ifndef _OPERATION_HH_
#define _OPERATION_HH_

#include <sstream>
#include <string>

#include "archive/ws_xml_oarchive.hpp"
#include "archive/ws_xml_iarchive.hpp"
#include "Exception.hh"
#include "Codec.hh"
#include "Context.hh"
#include <boost/algorithm/string.hpp>

namespace answer {

class Operation {
protected:
	std::string _name;
public:
	Operation(const std::string& name):_name(name){}
    virtual ~Operation() {};
    //The invocation wrapper
    virtual std::string invoke(const std::string&, const std::string & ="")=0;
};

//The default template is request / response
template <typename Type, typename OperationType, typename Request, typename Response, class Strategy  >
class OperationHandler: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op, const std::string &name):Operation(name), _op(op){}
	
	virtual std::string invoke ( const std::string& params, const std::string& prefix){
		std::ostringstream encodedReponse;
		std::auto_ptr<WebMethodException> wrappedException;
		try {
			Request request;
			
			size_t pos = _name.rfind("::");
			std::string filteredName(pos != _name.npos ? _name.substr(pos + 2) : _name);
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
			
			// we only look at the first accept. if it fails, we'll default to xml immediately
			std::string accept;
			const std::list<std::string>& accept_headers_list = answer::Context::getInstance().transportInfo().accepts();
			if (!accept_headers_list.empty()) {
				accept = accept_headers_list.front();
			}

			Type &type(_methodHandle.getInstance());

			Response response( (type.*_op)(request) );
			
			if(!codec::Codec(encodedReponse, accept, response)) {
				codec::GenericCodec(encodedReponse, response);
			}
		} catch (const WebMethodException &ex) {
			wrappedException = std::auto_ptr<WebMethodException>(new WebMethodException(ex.what()));
		} catch (const std::exception &ex) {
			wrappedException = std::auto_ptr<WebMethodException>(new WebMethodException("Framework exception"));
			std::cerr << "Exception: " << ex.what() << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error, attempting to proceed" <<std::endl;
		}
// 		std::cerr << "Debug:" << wrappedReponse.str() << std::endl;
		std::ostringstream wrappedReponse;
		codec::ResponseWrapper<void>(wrappedReponse, encodedReponse.str(), wrappedException.get());
		return wrappedReponse.str();
	}
};

//Partial specialization for Respose only
template <typename Type, typename OperationType, typename Response, class Strategy  >
class OperationHandler<Type, OperationType, boost::mpl::void_, Response, Strategy>: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op, const std::string &name):Operation(name), _op(op){}
public:
	virtual std::string invoke ( const std::string& , const std::string&){
		std::ostringstream encodedReponse;
		std::auto_ptr<WebMethodException> wrappedException;
		try {
			// we only look at the first accept. if it fails, we'll default to xml immediately
			std::string accept;
			const std::list<std::string>& accept_headers_list = answer::Context::getInstance().transportInfo().accepts();
			if (!accept_headers_list.empty()) {
				accept = accept_headers_list.front();
			}
			
			Type &type(_methodHandle.getInstance());
			
			Response response( (type.*_op)() );
			
			if(!codec::Codec(encodedReponse, accept, response)) {
				codec::GenericCodec(encodedReponse, response);
			}
		} catch (const WebMethodException &ex) {
			wrappedException = std::auto_ptr<WebMethodException>(new WebMethodException(ex.what()));
		} catch (const std::exception &ex) {
			wrappedException = std::auto_ptr<WebMethodException>(new WebMethodException("Framework exception"));
			std::cerr << "Exception: " << ex.what() << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error, attempting to proceed" <<std::endl;
		}
// 		std::cerr << "Debug:" << wrappedReponse.str() << std::endl;
		std::ostringstream wrappedReponse;
		codec::ResponseWrapper<void>(wrappedReponse, encodedReponse.str(), wrappedException.get());
		return wrappedReponse.str();
	}
};


//Partial specialization for Request only
template <typename Type, typename OperationType, typename Request, class Strategy  >
class OperationHandler<Type, OperationType, Request, void, Strategy>: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op, const std::string &name):Operation(name), _op(op){}

	virtual std::string invoke ( const std::string& params , const std::string& prefix){
		std::auto_ptr<WebMethodException> wrappedException;
		try {
			Request request;
			
			size_t pos = _name.rfind("::");
			std::string filteredName(pos != _name.npos ? _name.substr(pos + 2) : _name);
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

			Type &type(_methodHandle.getInstance());
			
			(type.*_op)(request);
		} catch (const WebMethodException &ex) {
			wrappedException = std::auto_ptr<WebMethodException>(new WebMethodException(ex.what()));
		} catch (const std::exception &ex) {
			wrappedException = std::auto_ptr<WebMethodException>(new WebMethodException("Framework exception"));
			std::cerr << "Exception: " << ex.what() << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error, attempting to proceed" <<std::endl;
		}
// 		std::cerr << "Debug No req:" << wrappedReponse.str() << std::endl;
		std::ostringstream wrappedReponse;
		codec::ResponseWrapper<void>(wrappedReponse, "", wrappedException.get());
		return wrappedReponse.str();
	}
};

}

#endif //_OPERATION_HH_
