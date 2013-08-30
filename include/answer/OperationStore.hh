#ifndef _OPERATION_STORE_HH_
#define _OPERATION_STORE_HH_

#include <map>
#include <string>

#include <boost/mpl/assert.hpp>
#include <boost/function.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/mpl/void.hpp>

#include "Operation.hh"
#include "InstantiationStrategy.hh"
#include "Macros.hh"
#include "ClassTemplateHelpers.hh"

namespace answer{

class OperationStore {
	std::map<std::string, Operation *> _map;
	OperationStore();

public:

	static OperationStore& Instance();

	~OperationStore();

	void registerOperation(const std::string& serviceName, const std::string& operationName, answer::Operation* webMethodHandle);
	Operation& operation(const std::string& serviceName, const std::string& operationName) const;

	std::list<std::string> operationList();
};

//TODO: RestPayload dies here... it's only read by for the service.xml REST Gateway
// Consider using just macros, REGISTER(AUTO) REGISTER_POST REGISTER_REST <- Future
template <typename Operation>
class RegisterOperation{
	std::string _operationName;
	std::string _serviceName;
public:
	RegisterOperation(const std::string& serviceName, const std::string& operationName, const Operation &op, RestPayload payLoad = AUTO):
		_operationName(operationName), _serviceName(serviceName)
	{
		//Unused parameter payload -> it's read by cpp_wsdl for service.xml generation
		(void)(payLoad);
		
		typedef typename class_<Operation>::type Type;

		typedef typename boost::function_types::result_type<Operation>::type
			response;
			
		typedef typename boost::mpl::at_c<boost::function_types::parameter_types<Operation>,1>::type response_type;

		typedef typename
			boost::remove_reference<
				response_type
			>::type const_request;

		typedef typename
			boost::remove_const<
				const_request
			>::type request;

		/*
		 * Get types of function
		 * request (const & request),
		 * where request is optional;
		 * 
		 *   response (void) and arity == 2 -> RequestOnly
		 *   response (!void) and arity == 2 -> RequestResponse
		 *   response (!void) and arity == 1 -> ResponseOnly
		 */

// Check operation signature (must have at least one of [response] operator()([request])
// 		BOOST_MPL_ASSERT(( boost::is_same<typex,void> ));
		try{
			OperationStore::Instance().registerOperation(_serviceName, _operationName, new OperationHandler<Type, Operation, request, response, InstantiationStrategy<Type> >(op, _operationName));
		}catch (std::exception &ex){
			std::cerr << "Error initializing operation ["<< _operationName << ": " << ex.what() << std::endl;
		}
	}

// 	~RegisterOperation(){
// 		OperationStore::Instance().removeOperation(_operationName);
// 	}
};
}
#endif //_OPERATION_STORE_HH_