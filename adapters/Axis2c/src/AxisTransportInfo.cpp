#include "AxisTransportInfo.hh"

namespace answer{
	namespace adapter{
		namespace axis{
			const std::string& AxisTransport::redirect(const std::string& uri){
				_uri = uri;
				return _uri;
			}
			const std::string& AxisTransport::redirect() const{
				return _uri;
			}
			
			bool AxisTransport::redirectSet() const{
				return !_uri.empty();
			}
		}
	}
}
