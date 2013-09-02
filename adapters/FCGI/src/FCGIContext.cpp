#include <FCGIContext.hh>

#include <boost/algorithm/string.hpp>

namespace answer{
namespace adapter{
namespace fcgi{
	
	using namespace std;

	FCGITransport::FCGITransport(const Fastcgipp::Http::Environment< char >& env){
		boost::split(_accepts, env.acceptContentTypes, boost::is_any_of(","), boost::token_compress_on);
		// remove "quality"
		for (list< string >::iterator itr = _accepts.begin(); itr != _accepts.end(); ++itr) {
			size_t pos = itr->find(";");
			if (pos != string::npos)
				*itr = itr->substr(0,pos);
		}
	}

	FCGIContext::FCGIContext(const Fastcgipp::Http::Environment< char > &env):
		_transport(env),
		_operation(env)
	{
	}

	FCGIContext::~FCGIContext()
	{

	}

	const list< string >& FCGITransport::accepts() const
	{
		return _accepts;
	}

	void FCGITransport::addHeader(const string& key, const string& value, bool replace)
	{

	}

	const string& FCGITransport::redirect() const
	{

	}

	const string& FCGITransport::redirect(const string& uri)
	{

	}

	bool FCGITransport::redirectSet() const
	{

	}

} //fcgi
} //adapter
} //answer
