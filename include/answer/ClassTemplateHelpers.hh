#ifndef _CLASS_TEMPLATE_HELPERS_HH_
#define _CLASS_TEMPLATE_HELPERS_HH_

#include "OperationStore.hh"

namespace answer{

// Declaration of general template
template<typename Pmf> struct class_;
// Partial specialisation for pointers to
// member functions

//Request response
template<typename Result, typename Class,
         typename Arg>
struct class_<Result (Class::*)(Arg)> {
  typedef Class type;
};
//Request response const
template<typename Result, typename Class,
         typename Arg>
struct class_<Result (Class::*)(Arg) const> {
  typedef Class type;
};

//Response
template<typename Result, typename Class>
struct class_<Result (Class::*)()> {
  typedef Class type;
};
//Response const
template<typename Result, typename Class>
struct class_<Result (Class::*)() const> {
  typedef Class type;
};

//Request
template<typename Class, typename Arg>
struct class_<void (Class::*)(Arg)> {
  typedef Class type;
};
//Request const
template<typename Class, typename Arg>
struct class_<void (Class::*)(Arg) const> {
  typedef Class type;
};

// boiler-plate
#define ANSWER_CONCATENATE_DETAIL(x, y) x##y
#define ANSWER_CONCATENATE(x, y) ANSWER_CONCATENATE_DETAIL(x, y)
#define ANSWER_MAKE_UNIQUE(x) ANSWER_CONCATENATE(x, __COUNTER__)

// The missing semicolon enforces the notion of simple replacement,
//  requiring the user use the macro as a function (which helps doxygen/cpp_wsdl xslt detection of registrations)

// The ANSWER_SERVICE_NAME definition should be provided on a per project basis,
//  the provided answer .cmake automatically set it to the project name
// #ifndef ANSWER_SERVICE_NAME
// #error "Answer: ANSWER_SERVICE_NAME has to be defined, usually done in the build definition"
// #endif //ANSWER_SERVICE_NAME
// Service registration macros
#define ANSWER_REGISTER_OPERATION(ServiceOperation) \
	answer::RegisterOperation<BOOST_TYPEOF(&ServiceOperation)> ANSWER_MAKE_UNIQUE(_registrator_)(ANSWER_SERVICE_NAME, #ServiceOperation, &ServiceOperation)

#define ANSWER_REGISTER_OPERATION_UNDER(ServiceOperation) \
	answer::RegisterOperation<BOOST_TYPEOF(&ServiceOperation)> ANSWER_MAKE_UNIQUE(_registrator_)(ANSWER_SERVICE_NAME, #ServiceOperation, &ServiceOperation)

#define ANSWER_REGISTER_OPERATION_POST(ServiceOperation) \
	answer::RegisterOperation<BOOST_TYPEOF(&ServiceOperation)> ANSWER_MAKE_UNIQUE(_registrator_)(ANSWER_SERVICE_NAME, #ServiceOperation, &ServiceOperation, answer::FORCE_POST)

}

#endif //_CLASS_TEMPLATE_HELPERS_HH_