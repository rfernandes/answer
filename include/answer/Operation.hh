
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
	
const std::string OK("OK");
const std::string ERROR("ERROR");

class Operation {
protected:
	std::string _name;
public:
	Operation(const std::string& name):_name(name){}
    virtual ~Operation() {};
    //The invocation wrapper
    virtual std::string invoke(const std::string&, const std::string & ="")=0;
};

std::string getResponseXml(int status_code, const std::string &status, const std::string &response = "");

//The default template is request / response
template <typename Type, typename OperationType, typename Request, typename Response, class Strategy  >
class OperationHandler: public Operation{
	Strategy _methodHandle;
	OperationType _op;
public:
	OperationHandler(OperationType op, const std::string &name):Operation(name), _op(op){}
	
	
	virtual std::string invoke ( const std::string& params, const std::string& prefix){
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

			std::ostringstream ssOut;
			if(!codec::Codec(ssOut, accept, response)) {
				codec::defaultCodec(ssOut, response);
				return getResponseXml(1000, OK, ssOut.str() );
			}
			return ssOut.str();
		} catch(const boost::archive::archive_exception& ex){
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
	OperationHandler(OperationType op, const std::string &name):Operation(name), _op(op){}
public:
	virtual std::string invoke ( const std::string& , const std::string&){
		try {
			// we only look at the first accept. if it fails, we'll default to xml immediately
			std::string accept;
			const std::list<std::string>& accept_headers_list = answer::Context::getInstance().transportInfo().accepts();
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
		} catch(const boost::archive::archive_exception& ex){
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
	OperationHandler(OperationType op, const std::string &name):Operation(name), _op(op){}

	virtual std::string invoke ( const std::string& params , const std::string& prefix){
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
			
			return getResponseXml(1000, OK);
			
		}catch(const boost::archive::archive_exception& ex){
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
