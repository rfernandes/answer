#define BOOST_TEST_MODULE ModuleTest
#include <boost/test/included/unit_test.hpp>

#include "answer/Module.hh"

using namespace std;

class TestModule: public answer::Module{
public:
	virtual FlowStatus inFlow ( answer::Context& context );
	virtual FlowStatus outFlow ( answer::Context& context );
	virtual FlowStatus outFlowFault ( answer::Context& context );
};

answer::Module::FlowStatus TestModule::inFlow ( answer::Context& context ) {
	return OK;
}

answer::Module::FlowStatus TestModule::outFlow ( answer::Context& context ) {
	return OK;
}

answer::Module::FlowStatus TestModule::outFlowFault ( answer::Context& context ) {
	return OK;
}

ANSWER_REGISTER_MODULE(TestModule);

BOOST_AUTO_TEST_CASE( module )
{
	BOOST_CHECK(true);
}

