#ifndef _UTILS_HH_
#define _UTILS_HH_

#include <string>
#include <ostream>
#include <boost/lexical_cast.hpp>
#include "Params.hh"

namespace answer{
		std::string encodeForXml( const std::string &sSrc , bool encode_special = true);
		std::string decodeFromXml( const std::string &sSrc );
}

#endif //_UTILS_HH_
