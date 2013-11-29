#ifndef _CONTEXT_HH_
#define _CONTEXT_HH_

#include <string>
#include <list>
#include "Environment.hh"
#include "Cookie.hh"

namespace answer {

class Context{
  static Context *_context;
  Response _response;
  Request _request;
public:
  virtual ~Context();
  virtual OperationInfo& operationInfo()=0;

<<<<<<< Updated upstream
=======
  const Request& request() const;
  void request(const Request &request);
  
>>>>>>> Stashed changes
  Response& response();
  void response(const Response &response);

  typedef std::vector<std::pair<std::string, std::string>> OrderedPairs;
  typedef std::map<std::string, std::string> Map;
  typedef std::map<std::string, Cookie> CookieMap;
<<<<<<< Updated upstream
=======
  typedef OrderedPairs QueryMap;
>>>>>>> Stashed changes
  typedef std::vector<std::string> Accepts;
  const Map& environment() const;
  
  void insert(const Cookie& cookie);
  const CookieMap& cookies() const;
  
  const Accepts& accepts() const;
<<<<<<< Updated upstream
=======
  void accepts(const answer::Context::Accepts &accepts);
  const QueryMap& query() const;
>>>>>>> Stashed changes
  
  static Context& Instance();
protected:
  Map _environment;
  CookieMap _cookies;
  Accepts _accepts;
<<<<<<< Updated upstream
=======
  QueryMap _query;
>>>>>>> Stashed changes
  Context();
};

}

#endif // _CONTEXT_HH_
