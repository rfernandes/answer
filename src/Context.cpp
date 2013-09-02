#include "answer/Context.hh"

namespace answer
{
	Context* Context::_context = nullptr;
	
	Context::Context(){
		if (_context){
			throw std::runtime_error("A context already exists");
		}else{
			_context = this;
		}
	}
	
	Context& Context::Instance() {
		if ( !_context ) {
			throw std::runtime_error ( "Uninitialized context" );
		}
		return *_context;
	}
	
}

