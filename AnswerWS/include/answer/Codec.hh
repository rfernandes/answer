#ifndef _CODECSTORE_HH_
#define _CODECSTORE_HH_

#include <string>
#include <iostream>
#include "serialization/ws_xml_oarchive.hpp"

namespace answer{
	namespace codec{
		template<typename T>
		void defaultCodec ( std::ostream& out, const T& data)
		{
// 				std::cerr << "DEFAULT CODEC" << std::endl;
			ws_xml_oarchive outA ( out );
			outA << data;
		}

		template<typename T>
		bool Codec ( std::ostream&, const std::string&, const T&)
		{
// 				std::cerr << "NO CODEC" << std::endl;
			return false;
		}
	}
}

#endif // _CODECSTORE_HH_