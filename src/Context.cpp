#include "answer/Context.hh"

using namespace std;

namespace answer
{

Context *Context::_context = nullptr;

Context::Context()
{
  if (_context)
  {
    throw runtime_error("A context already exists");
  }
  else
  {
    _context = this;
  }
}

Context &Context::Instance()
{
  if (!_context)
  {
    throw runtime_error("Uninitialized context");
  }
  return *_context;
}

Context::~Context()
{
  if (_context)
  {
    _context = nullptr;
  }
}

Response &Context::response()
{
  return _response;
}

void Context::response(const Response &val)
{
  _response = val;
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
const Context::Accepts &Context::accepts() const
{
  return _accepts;
}

void Context::request(const Request &request)
{
  _request = request;
}
const Request &Context::request() const
{
  return _request;
}
const Context::QueryMap &Context::query() const
{
  return _query;
}
void Context::accepts(const Context::Accepts &accepts)
{
  _accepts = accepts;
}

void Context::accepts(const string &accepts)
{
  string mime;
  bool params = false;
  for (auto c : accepts)
  {
    switch (c)
    {
      case ' ': // trim
        break;
      case ';': // ignore accept-params
        _accepts.emplace_back(mime);
        params = true;
        mime.clear();
        break;
      case ',':
        if (params)
        {
          params = false;
          mime.clear();
          continue;
        }
        _accepts.emplace_back(mime);
        cerr << mime << endl;
        mime.clear();
        break;
      default:
        mime.push_back(c);
        break;
    }
  }
  if (!params)
    _accepts.emplace_back(mime);
}


}

