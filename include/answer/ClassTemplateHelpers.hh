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
#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define MAKE_UNIQUE(x) CONCATENATE(x, __COUNTER__)

// The missing semicolon enforces the notion of simple replacement,
//  requiring the user use the macro as a function (which helps doxygen/cpp_wsdl xslt detection of registrations)

// Service registration macros
#define REGISTER_OPERATION(ServiceOperation) \
	answer::RegisterOperation<BOOST_TYPEOF(&ServiceOperation)> MAKE_UNIQUE(_registrator_)(#ServiceOperation, &ServiceOperation)

#define REGISTER_OPERATION_UNDER(ServiceOperation) \
	answer::RegisterOperation<BOOST_TYPEOF(&ServiceOperation)> MAKE_UNIQUE(_registrator_)(#ServiceOperation, &ServiceOperation)

	
#define REGISTER_OPERATION_POST(ServiceOperation) \
	answer::RegisterOperation<BOOST_TYPEOF(&ServiceOperation)> MAKE_UNIQUE(_registrator_)(#ServiceOperation, &ServiceOperation, answer::FORCE_POST)

}

#endif //_CLASS_TEMPLATE_HELPERS_HH_