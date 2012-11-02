
#ifndef _OPERATION_HH_
#define _OPERATION_HH_

#include <sstream>
#include <string>

#include "serialization/ws_xml_oarchive.hpp"
#include "serialization/ws_xml_iarchive.hpp"
#include "Exception.hh"
#include "Codec.hh"
#include "Context.hh"
#include <boost/algorithm/string.hpp>
#include <apr-1/apr_poll.h>

namespace answer {
	
const std::string OK("OK");
const std::string ERROR("ERROR");

class Operation {
public:
	Operation() {}
    virtual ~Operation() {};
    //The invocation wrapper
    virtual std::string invoke(const std::string&)=0;
};

std::string getResponseXml(int status_code, const std::string &status, const std::string &response = "");

//The default template is request / response
template <typename Type, typename OperationType, typename Request, typename Response, class Strategy  >
class OperationHandler: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op):_op(op){}
	
	virtual std::string invoke ( const std::string& params ){
		try {
			Request request;

			std::istringstream ssIn(params);
			{
				ws_xml_iarchive inA(ssIn);
				inA >> request;
			}
			
			// we only look at the first accept. if it fails, we'll default to xml immediately
			std::string accept;
			const std::list<std::string>& accept_headers_list = answer::Context::getInstance().request().getAcceptList();
			if (!accept_headers_list.empty()) {
				accept = accept_headers_list.front();
			}
// 			std::cerr << "encoding response for: " << accept << std::endl;

			Type &type(_methodHandle.getInstance());
			
			Response response( (type.*_op)(request) );

			std::ostringstream ssOut;
			if(!codec::Codec(ssOut, accept, response)) {
				codec::defaultCodec(ssOut, response);
				return getResponseXml(1000, OK, ssOut.str() );
			}
			return ssOut.str();
		} catch(const xmlpp::exception& ex){
			return getResponseXml(1002, ex.what());
		} catch (const WebMethodMissingParameter &ex) {
			return getResponseXml(1010, ex.what());
		} catch (const WebMethodInvalidInput &ex) {
			return getResponseXml(1003, ex.what());
		} catch (const WebMethodException &ex) {
			return getResponseXml(ex.getErrorLevel(), ex.what());
		} catch (const std::exception &ex) {
			std::cerr << "Exception: " << ex.what() << "returning generic error message" << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error" <<std::endl;
		}
		return getResponseXml(1002, ERROR);
	}
};

//Partial specialization for Respose only
template <typename Type, typename OperationType, typename Response, class Strategy  >
class OperationHandler<Type, OperationType, boost::mpl::void_, Response, Strategy>: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op):_op(op){}
public:
	virtual std::string invoke ( const std::string& ){
		try {
			// we only look at the first accept. if it fails, we'll default to xml immediately
			std::string accept;
			const std::list<std::string>& accept_headers_list = answer::Context::getInstance().request().getAcceptList();
			if (!accept_headers_list.empty()) {
				accept = accept_headers_list.front();
			}
			
			Type &type(_methodHandle.getInstance());
			
			Response response( (type.*_op)() );
			
			std::ostringstream ssOut;
			if(!codec::Codec(ssOut, accept, response)) {
				codec::defaultCodec(ssOut, response);
				return getResponseXml(1000, OK, ssOut.str() );
			}
			return ssOut.str();
		} catch(const xmlpp::exception& ex){
			return getResponseXml(1002, ex.what());
		} catch (const WebMethodMissingParameter &ex) {
			return getResponseXml(1010, ex.what());
		} catch (const WebMethodInvalidInput &ex) {
			return getResponseXml(1003, ex.what());
		} catch (const WebMethodException &ex) {
			return getResponseXml(ex.getErrorLevel(), ex.what());
		} catch (const std::exception &ex) {
			std::cerr << "Exception: " << ex.what() << "returning generic error message" << std::endl;
		} catch (...){
			std::cerr << "Catastrophic error" <<std::endl;
		}
		return getResponseXml(1002, ERROR);
	}
};


//Partial specialization for Request only
template <typename Type, typename OperationType, typename Request, class Strategy  >
class OperationHandler<Type, OperationType, Request, void, Strategy>: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op):_op(op){}

	virtual std::string invoke ( const std::string& params ){
		try {
			Request request;
			
			std::istringstream ssIn(params);
			{
				ws_xml_iarchive inA(ssIn);
				inA >> request;
			}
			
			Type &type(_methodHandle.getInstance());
			
			(type.*_op)(request);
			
			return getResponseXml(1000, OK);
			
		}catch(const xmlpp::exception& ex){
			return getResponseXml(1002, ex.what());
		} catch (const WebMethodException &ex) {
			return getResponseXml(ex.getErrorLevel(), ex.what());
		} catch (const std::exception &ex) {
			std::cerr << "Exception: " << ex.what() << "returning generic error message" <<std::endl;
		} catch (...){
			std::cerr << "Catastrophic error" <<std::endl;
		}
		return getResponseXml(1002, ERROR);
	}
};

}

#endif //_OPERATION_HH_
