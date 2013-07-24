
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
#include <stdexcept>
#include <exception>

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


//The default template is request / response
template <typename Type, typename OperationType, typename RequestT, typename ResponseT, class Strategy  >
class OperationHandler: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op, const std::string &name):Operation(name), _op(op){}
	
	virtual Response invoke ( const std::string& params, const std::string& prefix){
		Response ret;
		std::ostringstream wrappedReponse;
		try {
			RequestT request;
			
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

			ResponseT response( (type.*_op)(request) );

			const std::list<std::string>& accept_headers_list = answer::Context::getInstance().transportInfo().accepts();
			std::list<std::string>::const_iterator it = accept_headers_list.begin();
			
			std::ostringstream encodedReponse;
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
			if (!codec::ResponseWrapper<void>(wrappedReponse, encodedReponse.str(), ret.acceptType, NULL)){
				codec::ResponseWrapper<char>(wrappedReponse, encodedReponse.str(), ret.acceptType, NULL);
			}
		} catch (const WebMethodException &ex) {
			if (!codec::ResponseWrapper<void>(wrappedReponse, "", ret.acceptType, &ex)){
				codec::ResponseWrapper<char>(wrappedReponse, "", ret.acceptType, &ex);
			}
		} catch (const std::exception &ex) {
			wrappedException = std::auto_ptr<WebMethodException>(new WebMethodException("Framework exception"));
			std::cerr << "Exception: " << ex.what() << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error, attempting to proceed" <<std::endl;
		}
// 		std::cerr << "Debug:" << wrappedReponse.str() << std::endl;
		ret.response = wrappedReponse.str();

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
		std::ostringstream wrappedReponse;
		try {
			Type &type(_methodHandle.getInstance());
			
			ResponseT response( (type.*_op)() );
			
			// we only look at the first accept. if it fails, we'll default to xml immediately
			const std::list<std::string>& accept_headers_list = answer::Context::getInstance().transportInfo().accepts();
			std::list<std::string>::const_iterator it = accept_headers_list.begin();
			
			std::ostringstream encodedReponse;
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
			//  implementation (Assume no char template is defined, default is instanciated)
			if (!codec::ResponseWrapper<void>(wrappedReponse, encodedReponse.str(), ret.acceptType, NULL)){
				codec::ResponseWrapper<char>(wrappedReponse, encodedReponse.str(), ret.acceptType, NULL);
			}
		} catch (const WebMethodException &ex) {
			if (!codec::ResponseWrapper<void>(wrappedReponse, "", ret.acceptType, &ex)){
				codec::ResponseWrapper<char>(wrappedReponse, "", ret.acceptType, &ex);
			}
		} catch (const std::exception &ex) {
			wrappedException = std::auto_ptr<WebMethodException>(new WebMethodException("Framework exception"));
			std::cerr << "Exception: " << ex.what() << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error, attempting to proceed" <<std::endl;
		}
// 		std::cerr << "Debug:" << wrappedReponse.str() << std::endl;
		ret.response = wrappedReponse.str();
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
			RequestT request;
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
// 			codec::ResponseWrapper<void>(wrappedReponse, "", "", NULL);
		} catch (const WebMethodException &ex) {
			//TODO: Tricky case, no response expected but webexception thrown. Loggin for now
			std::cerr << "WebException: " << ex.what() << std::endl;
// 			codec::ResponseWrapper<void>(wrappedReponse, "", "", &ex);
		} catch (const std::exception &ex) {
			wrappedException = std::auto_ptr<WebMethodException>(new WebMethodException("Framework exception"));
			std::cerr << "Exception: " << ex.what() << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error, attempting to proceed" <<std::endl;
		}
// 		std::cerr << "Debug No req:" << wrappedReponse.str() << std::endl;
// 		ret.response=wrappedReponse.str(); //TODO: Speficy wrapper accept used
		return ret;
	}
};

}

#endif //_OPERATION_HH_
