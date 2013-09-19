#ifndef _ENVIRONMENT_HH_
#define _ENVIRONMENT_HH_

#include <string>
#include <list>
#include <boost/any.hpp>
#include "Cookie.hh"

namespace answer{
	class Environment{
	public:
		virtual void insert(const std::string& key, const std::string &value) = 0;
		virtual std::string at(const std::string&key) const = 0;
		virtual bool contains(const std::string &key) const = 0;
	};
	
	class ProviderStore{
	public:
		virtual void addProvider(const std::string& key, const boost::any& value) = 0;
		virtual const boost::any& getProvider(const std::string& key) const = 0;
		virtual bool contains(const std::string &key) const = 0;
	};
	
	class OperationInfo{
	public:
		virtual const std::string& service() const = 0;
		virtual const std::string& operation() const = 0;
		virtual const std::string& url() const = 0;
	};

  class Response{
	public:
		typedef std::pair<std::string, std::string> Header;
  protected:
    std::string _body;
    std::string _contentType;
		std::vector<Header> _headers;
  public:
    //TODO: Add vector<char> data accessors for body
    const std::string &body() const;
    void body(const std::string &body);
    const std::string &contentType() const;
    void contentType(const std::string &contentType);

		//header
    void header(const std::string &key, const std::string &value);
    const std::vector<Header> & headers() const;
  };

	class TransportInfo{
	private:
		bool _binaryTransport;
  protected:
	public:
		TransportInfo() : _binaryTransport(false) {}
		virtual void addHeader(const std::string &key, const std::string &value = "", bool replace = true) = 0;
		virtual const std::list<std::string>& accepts() const = 0;
		virtual const std::string& redirect(const std::string& uri) = 0;
		virtual const std::string& redirect() const = 0;
		virtual bool redirectSet() const = 0;
		virtual inline bool isBinaryTransport() const { return _binaryTransport; }
		virtual inline void setBinaryTransport() { _binaryTransport = true; }
	};

	class CookieJar{
	public:
		virtual void insert(const Cookie& cookie, bool isNew = true) = 0;
		virtual void remove(const std::string&cookieName) = 0;
		
		virtual std::list<Cookie> list(bool onlyNew) const = 0;
		
		virtual const Cookie& cookie(const std::string &cookieName) const = 0;
		virtual bool contains(const std::string &cookieName) const = 0;
	};
}

#endif
