#include "answer/archive/ws_xml_iarchive.hpp"
#include <boost/archive/detail/archive_serializer_map.hpp>

// explicitly instantiate for this type of xml stream
#include <boost/archive/impl/archive_serializer_map.ipp>
#include <boost/archive/impl/basic_xml_iarchive.ipp>
#include <boost/archive/impl/xml_iarchive_impl.ipp>

namespace answer{
namespace archive{
	template <>
	void ws_xml_iarchive::load_override(
		#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
		const
		#endif
		boost::serialization::nvp< bool > & t, int){
		std::string str;
		base::load_override(boost::serialization::make_nvp<std::string>(t.name(), str), 0);
		t.value() = (str == "true");
	}
}
}

namespace boost {
namespace archive {

template class detail::archive_serializer_map<answer::archive::ws_xml_iarchive>;
template class basic_xml_iarchive<answer::archive::ws_xml_iarchive> ;
template class xml_iarchive_impl<answer::archive::ws_xml_iarchive> ;

}
}
