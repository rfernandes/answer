#include "answer/Cookie.hh"

namespace answer{
	
static const char * hexa_digits = "0123456789ABCDEF";
std::string UrlEncode ( const std::string& orig ) 
{
	unsigned char ch;
	char * dest = new char[orig.size()*3+1]; // worst case scenario: %xx for all chars (highly unlikely, but lets be safe)
	char * start = dest;
	for ( std::string::const_iterator it = orig.begin(); it != orig.end(); ++it ) 
	{
		ch = ( unsigned char ) *it;
		if ( isalnum ( ch ) || strchr ( "-_.!~*'()", ch ) ) 
		{
			*dest++ = *it;
		}
		else 
		{
			*dest++ = '%';
			*dest++ = hexa_digits[( ch >> 4 ) & 0x0F];
			*dest++ = hexa_digits[ch & 0x0F];
		}
	}
	*dest = 0;
	std::string ret( start );
	delete[] start;
	return ret;
}

const std::string& Cookie::value() const
{
	return _value;
}

void Cookie::value(const std::string& value)
{
	_value = value;
}

const Cookie::Expires& Cookie::expires() const
{
	return _expires;
}

void Cookie::expires(const answer::Cookie::Expires& expires)
{
	_expires = expires;
}

bool Cookie::secure() const
{
	return _secure;
}

void Cookie::secure(bool secure)
{
	_secure = secure;
}

bool Cookie::httpOnly() const
{
	return _httpOnly;
}

void Cookie::httpOnly(bool httpOnly)
{
	_httpOnly = httpOnly;
}

Cookie::Cookie ( const std::string& name, const std::string& value, const std::string& path, const answer::Cookie::Expires& expires, bool secure, bool httpOnly) :
	_name ( UrlEncode(name) ),
	_value ( UrlEncode(value) ),
	_path ( path ),
	_expires ( expires ),
	_secure ( secure ),
	_httpOnly ( httpOnly )
{}

std::string convert ( const boost::posix_time::ptime& t )
{
	std::ostringstream ss;
	ss.exceptions ( std::ios_base::failbit );
	
	boost::date_time::time_facet<boost::posix_time::ptime, char>* facet = new boost::date_time::time_facet<boost::posix_time::ptime, char>;
	ss.imbue ( std::locale ( std::locale::classic(), facet ) );
	
	facet->format ( "%a, %d-%b-%Y %T GMT" );
	ss.str ( "" );
	ss << t;
	return ss.str();
}

const std::string& Cookie::name() const{
	return _name;
}

void Cookie::name(const std::string& name){
	_name = name;
}

Cookie::Expires::Expires () : _expires() {}

Cookie::Expires::Expires ( const std::string& gmt_string ): _expires ( convert ( boost::posix_time::time_from_string(gmt_string) ) ) {}

Cookie::Expires::Expires ( time_t t ): _expires ( convert ( boost::posix_time::from_time_t ( t ) ) ) {}

Cookie::Expires::Expires ( struct tm tm_ )
{
	_expires = convert(
		boost::posix_time::ptime ( 
			boost::gregorian::date ( tm_.tm_year, tm_.tm_mon, tm_.tm_mday ), 
			boost::posix_time::time_duration ( tm_.tm_hour, tm_.tm_min, tm_.tm_sec ) 
		) 
	);
}

Cookie::Expires::Expires ( const boost::posix_time::ptime& expires ) : _expires ( convert ( expires ) ) {}

Cookie::Expires::Expires ( const Expires& e ) : _expires ( e._expires ) {}

Cookie::Expires::~Expires () throw() {}

Cookie::Expires& Cookie::Expires::operator= ( const Expires& e )
{
	_expires = e._expires;
	return *this;
}

bool Cookie::Expires::empty() const
{
	return _expires.empty();
}


std::ostream &operator<<(std::ostream &out, const Cookie &cookie)
{
  out << cookie._name << "=" << cookie._value << "; Path=" << cookie._path;
  if ( !cookie._expires.empty() ){
    out << "; Expires=" << cookie._expires;
  }
  if ( cookie._secure ) {
    out << "; Secure";
  }
  if ( cookie._httpOnly ) {
    out << "; HttpOnly";
  }
  return out;
}


std::ostream &operator<<(std::ostream &out, const Cookie::Expires &exp)
{
  out << exp._expires;
  return out;
}

}

