#include "answer/archive/ws_xml_oarchive.hpp"
#include <boost/archive/detail/archive_serializer_map.hpp>

// explicitly instantiate for this type of xml stream
#include <boost/archive/impl/archive_serializer_map.ipp>
#include <boost/archive/impl/basic_xml_oarchive.ipp>
#include <boost/archive/impl/xml_oarchive_impl.ipp>

namespace answer{
namespace archive{
	template <>
	void ws_xml_oarchive::save_override(const boost::serialization::nvp< bool > & t, int){
		std::string aux(t.value() ? "true": "false");
		//Serialize the string
		* this << boost::serialization::make_nvp(t.name(), aux);
	}
}
}

namespace boost {
namespace archive {

template class detail::archive_serializer_map<answer::archive::ws_xml_oarchive>;
template class basic_xml_oarchive<answer::archive::ws_xml_oarchive> ;
template class xml_oarchive_impl<answer::archive::ws_xml_oarchive> ;

}
}