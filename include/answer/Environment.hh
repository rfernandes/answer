#ifndef _ENVIRONMENT_HH_
#define _ENVIRONMENT_HH_

#include <string>
#include <list>
#include <boost/any.hpp>
#include "answer/Cookie.hh"

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
		virtual const std::string& getServiceName() const = 0;
		virtual const std::string& getOperationName() const = 0;
		virtual const std::string& getName() const = 0;
		virtual const std::string& getURL() const = 0;
	};
	
	class TransportInfo{
	private:
		bool _binaryTransport;
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
		
		virtual const Cookie& at(const std::string&cookieName) const = 0;
		virtual bool contains(const std::string &cookieName) const = 0;
	};
}

#endif
