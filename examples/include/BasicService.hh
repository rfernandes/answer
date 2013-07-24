#ifndef _BASIC_SERVICE_HH_
#define _BASIC_SERVICE_HH_
#include <answer/OperationStore.hh>
#include <string>
#include <boost/serialization/list.hpp>

///Basic type service testing
namespace WebServices{

	bool responseWrapper( std::ostream& out, const std::string& response, const std::string& mimetype, const answer::WebMethodException* ex){
		if (mimetype != "text/xml") return false;
		
		out << "<return>"
		<< response
		<< "<status>"
		<< "<status_code>" << (ex?ex->getErrorLevel():1000) << "</status_code>"
		<< "<status_message>" <<  (ex?ex->what():"OK")  << "</status_message>"
		<< "</status>"
		<< "</return>";
		return true;
	}

	struct BasicOperationResponse{
		std::list<std::string> items;
		
		template<class Archive>
		void serialize(Archive & ar, const unsigned int /*version*/)
		{
			ar & BOOST_SERIALIZATION_NVP(items);
		}
	};
	
  struct testInput{
    std::set<unsigned> test;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int /*version*/)
    {
      ar & BOOST_SERIALIZATION_NVP(test);
    }
  };
	
	class BasicService{
	public:
		~BasicService(){std::cerr << "Bye" << std::endl;}
		BasicService(){std::cerr << "Hi" << std::endl;}

		std::string hello_world();
    void testOperation(const testInput& test);
		BasicOperationResponse simple_operation();
	};
}

ANSWER_RESPONSE_WRAPPER(WebServices::responseWrapper);
ANSWER_REGISTER_OPERATION(WebServices::BasicService::hello_world);
ANSWER_REGISTER_OPERATION(WebServices::BasicService::simple_operation);
ANSWER_REGISTER_OPERATION(WebServices::BasicService::testOperation);

#endif //_BASIC_SERVICE_HH_