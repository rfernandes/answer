#ifndef _COOKIE_HH_
#define _COOKIE_HH_

#include <string>
#include <time.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace answer{

class Cookie
{
public:
	class Expires
	{
	private:
		std::string _expires;
		
	public:
		Expires ();
		Expires (const std::string& gmt_string);
		Expires (time_t t);
		Expires (struct tm tm_);
		Expires (const boost::posix_time::ptime& expires);
		Expires (const Expires&);
		~Expires () throw();
		
		Expires& operator=(const Expires&);
		
		friend std::ostream& operator<< (std::ostream &out, const Expires& exp) {
			out << exp._expires;
			return out;
		}
		bool empty() const;
	};
	
	Cookie(const std::string& name, const std::string& value, const std::string& path = "/", const Expires& expires = Expires(), bool secure = false, bool httponly = false);

	operator std::string() const;
	
	const std::string& name() const;
	void name(const std::string & str);
	
	const std::string& value() const;
	void value(const std::string& value);

	const Expires& expires() const;
	void expires(const Expires&);
	
	bool secure() const;
	void secure(bool secure);
	
	bool httpOnly() const;
	void httpOnly(bool httpOnly);

private:
	std::string	_name;
	std::string	_value;
	std::string	_path;
	Expires		_expires;
	bool			_secure, _httpOnly;
};

}

#endif // _COOKIE_HH_
