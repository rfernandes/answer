#ifndef _AXIS_TRANSPORT_HH_
#define _AXIS_TRANSPORT_HH_

#include "answer/Environment.hh"

namespace answer{
	namespace adapter{
		namespace axis{

			class AxisTransport: public TransportInfo {
				std::string _uri;
			public:
				virtual const std::string& redirect(const std::string& uri);
				virtual const std::string& redirect() const;
				virtual bool redirectSet() const;
			};

		}
	}
}

#endif //_AXIS_OPERATION_HH_