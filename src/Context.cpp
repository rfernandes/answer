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

Context::~Context(){
  if (_context){
    _context = nullptr;
  }
}

Response &Context::response()
{
  return _response;
}

void Context::response(const Response &response)
{
  _response = response;
}
const Context::Map &Context::environment() const
{
  return _environment;
}
const Context::CookieMap &Context::cookies() const
{
  return _cookies;
}
void Context::insert(const Cookie &cookie)
{
  _cookies.insert(make_pair(cookie.name(), cookie));
}


}

