#ifndef _ENVIRONMENT_HH_
#define _ENVIRONMENT_HH_

#include <string>
#include <list>
#include <map>
#include <boost/any.hpp>
#include "Cookie.hh"

namespace answer
{

class OperationInfo
{
  std::string _operation;
  std::string _service;
  std::string _url;
public:
  OperationInfo(const std::string url);
  
  const std::string &operation() const;
  const std::string &service() const;
  const std::string &url() const;
};

class Request
{
  std::string _body;
public:
  const std::string &body() const;
  void body(const std::string &body);
};

class Response
{
public:
  typedef std::pair<std::string, std::string> Header;

  enum class Status
  {
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    FOUND = 302,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500
  };
  const static std::map<Status, std::string> statusText;

protected:
  std::string _body;
  std::string _contentType;
  std::vector<Header> _headers;
  Status _status = Status::OK;
public:
  const std::string &body() const;
  void body(const std::string &body);
  const std::string &contentType() const;
  void contentType(const std::string &contentType);

  //header
  void header(const std::string &key, const std::string &value);
  const std::vector<Header> &headers() const;

  void status(Status status);
  Status status() const;
};

}

#endif
