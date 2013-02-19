#include "answer/archive/ws_xml_iarchive.hpp"
#include <boost/archive/detail/archive_serializer_map.hpp>

// explicitly instantiate for this type of xml stream
#include <boost/archive/impl/archive_serializer_map.ipp>
#include <boost/archive/impl/basic_xml_iarchive.ipp>
#include <boost/archive/impl/xml_iarchive_impl.ipp>

namespace boost {
namespace archive {

template class detail::archive_serializer_map<answer::archive::ws_xml_iarchive>;
template class basic_xml_iarchive<answer::archive::ws_xml_iarchive> ;
template class xml_iarchive_impl<answer::archive::ws_xml_iarchive> ;

}
}
