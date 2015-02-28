#ifndef _CONTEXT_HH_
#define _CONTEXT_HH_

#include <string>
#include <list>
#include "Environment.hh"
#include "Cookie.hh"

namespace answer
{

class Context
{
  static Context *_context;
  Response _response;
  Request _request;
  
public:
  virtual ~Context();
  virtual OperationInfo &operationInfo() = 0;

  const Request &request() const;
  void request(const Request &request);

  Response &response();
  void response(const Response &response);

  using OrderedPairs = std::vector<std::pair<std::string, std::string>>;
  using Map = std::map<std::string, std::string>;
  using CookieMap = std::map<std::string, Cookie>;
  using QueryMap = OrderedPairs;
  using Accepts = std::vector<std::string>;
  const Map &environment() const;

  void insert(const Cookie &cookie);
  const CookieMap &cookies() const;

  const Accepts &accepts() const;
  void accepts(const answer::Context::Accepts &accepts);
  void accepts(const std::string &accepts);
  const QueryMap &query() const;

  static Context &Instance();
protected:
  Map _environment;
  CookieMap _cookies;
  Accepts _accepts;
  QueryMap _query;
  Context();
};

}

#endif // _CONTEXT_HH_
