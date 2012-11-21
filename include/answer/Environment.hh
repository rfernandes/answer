#ifndef _ENVIRONMENT_HH_
#define _ENVIRONMENT_HH_

#include <string>
#include <boost/any.hpp>

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
}

#endif