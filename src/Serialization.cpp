#include "answer/serialization/ws_xml_oarchive.hpp"
#include "answer/serialization/ws_xml_iarchive.hpp"


template<>
ws_xml_oarchive & ws_xml_oarchive::operator<<(const boost::serialization::nvp< boost::serialization::collection_size_type > & t){
	m_collectionStack.push_back({m_currentName, t.value(), true});
	return *this;
}

template<>
ws_xml_iarchive & ws_xml_iarchive::operator>>(const boost::serialization::nvp< boost::serialization::collection_size_type > & t){
	t.value() = _currElem->get_parent()->find(m_currentName).size();
	m_collectionStack.push_back({m_currentName, t.value() , true});
	return *this;
}