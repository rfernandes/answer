#include "answer/archive/ws_xml_oarchive.hpp"
#include <boost/archive/detail/archive_serializer_map.hpp>

// explicitly instantiate for this type of xml stream
#include <boost/archive/impl/archive_serializer_map.ipp>
#include <boost/archive/impl/basic_xml_oarchive.ipp>
#include <boost/archive/impl/xml_oarchive_impl.ipp>

namespace boost
{
namespace archive
{

template class detail::archive_serializer_map<answer::archive::ws_xml_oarchive>;
template class basic_xml_oarchive<answer::archive::ws_xml_oarchive> ;
template class xml_oarchive_impl<answer::archive::ws_xml_oarchive> ;

}
}
