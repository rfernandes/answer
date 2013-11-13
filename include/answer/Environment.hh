#ifndef _ENVIRONMENT_HH_
#define _ENVIRONMENT_HH_

#include <string>
#include <list>
#include <map>
#include <boost/any.hpp>
#include "Cookie.hh"

namespace answer{
	
  class OperationInfo{
  public:
    virtual const std::string& service() const = 0;
    virtual const std::string& operation() const = 0;
    virtual const std::string& url() const = 0;
  };

  class Response{
  public:
    typedef std::pair<std::string, std::string> Header;
    //TODO: Complete this list
    enum class Status {
      OK_,
      CREATED = 201,
      ACCEPTED = 202,
      NOT_FOUND = 404
    };
    const static std::map<Status, std::string> statusText;

  protected:
    std::string _body;
    std::string _contentType;
    std::vector<Header> _headers;
    Status _status = Status::OK_;
  public:
    //TODO: Add vector<char> data accessors for body
    const std::string &body() const;
    void body(const std::string &body);
    const std::string &contentType() const;
    void contentType(const std::string &contentType);

    //header
    void header(const std::string &key, const std::string &value);
    const std::vector<Header> & headers() const;

    void status(Status status);
    Status status() const;
  };

  class TransportInfo{
  private:
    bool _binaryTransport;
  protected:
  public:
    TransportInfo() : _binaryTransport(false) {}
    virtual const std::list<std::string>& accepts() const = 0;
    virtual const std::string& redirect(const std::string& uri) = 0;
    virtual const std::string& redirect() const = 0;
    virtual bool redirectSet() const = 0;
    virtual inline bool isBinaryTransport() const { return _binaryTransport; }
    virtual inline void setBinaryTransport() { _binaryTransport = true; }
  };

}

#endif
