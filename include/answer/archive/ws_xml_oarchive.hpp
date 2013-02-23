#ifndef ANSWER_WS_XML_OARCHIVE_HPP
#define ANSWER_WS_XML_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <list>
#include <boost/optional.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/concept_check.hpp>

namespace answer{
namespace archive{

/////////////////////////////////////////////////////////////////////////
// log data to an output stream.  This illustrates a simpler implemenation
// of text output which is useful for getting a formatted display of
// any serializable class.  Intended to be useful as a debugging aid.
class ws_xml_oarchive :
    public boost::archive::xml_oarchive_impl<ws_xml_oarchive>
{
	std::string m_currentName;
	struct collectionEntry{
		std::string itemName;
		size_t size;
		bool newEntry;
	};
	std::list<collectionEntry> m_collectionStack;

    typedef boost::archive::xml_oarchive_impl<ws_xml_oarchive> base;
    // give serialization implementation access to this clas
    friend class boost::archive::detail::interface_oarchive<ws_xml_oarchive>;
    friend class boost::archive::basic_xml_oarchive<ws_xml_oarchive>;
    friend class boost::archive::save_access;

    /////////////////////////////////////////////////////////////////////
    // Override functions defined in basic_xml_oarchive

    // Anything not an attribute and not a name-value pair is an
    // error and should be trapped here.  Note the usage of
    // function argument matching (BOOST_PFTO) to permit usage
    // with compilers which fail to properly support 
    // function template ordering
    template<class T>
    void save_override(T & t, BOOST_PFTO int){
        // make it a name-value pair and pass it on.
        // this permits this to be used even with data types which
        // are not wrapped with the name
        base::save_override(boost::serialization::make_nvp(NULL, t), 0);
    }

    void save_override(const boost::serialization::nvp< boost::serialization::collection_size_type > & t, int){
			collectionEntry aux;
			aux.itemName = m_currentName;
			aux.size = t.value();
			aux.newEntry = true;
			m_collectionStack.push_back(aux);
    }
    template<class T>
    void save_override(const boost::serialization::nvp< T > & t, int){
			std::string itemName (t.name());
			bool isLastEntry = false;
			bool isNewEntry = false;
			if (!m_collectionStack.empty() ){
				if (itemName == "item_version")
					return;
				if (itemName == "item"){
					if (m_collectionStack.back().newEntry){
						m_collectionStack.back().newEntry = false;
						isNewEntry = true;
					}
					itemName = m_collectionStack.back().itemName;
					if (--m_collectionStack.back().size == 0){
						m_collectionStack.pop_back();
						isLastEntry = true;
					}
				}
			}
// 			t.name() = itemName.c_str();
			m_currentName = itemName;
			if (!isNewEntry){
// 				return;
				os << '<' << itemName << '>';
			}
			* this << t.const_value();
			if (!isLastEntry){
				os << "</" << itemName << '>';
			}
			// this is here to remove the "const" requirement.  Since
			// this class is to be used only for output, it's not required.
// 			base::save_override(boost::serialization::make_nvp(itemName.c_str(), t.value()), 0);
    }
    
    
    template<class T>
    void save_override(const boost::serialization::nvp< boost::optional<T> > & t, int){
// 			boost::optional x;
			if (t.value())
				* this << boost::serialization::make_nvp(t.name(), t.value().get());
		}
    
    // specific overrides for attributes - not name value pairs so we
    // want to trap them before the above "fall through"
    // since we don't want to see these in the output - make them no-ops.
    void save_override(const boost::archive::object_id_type & t, int){}
    void save_override(const boost::archive::object_reference_type & t, int){}
    void save_override(const boost::archive::version_type & t, int){}
    void save_override(const boost::archive::class_id_type & t, int){}
    void save_override(const boost::archive::class_id_optional_type & t, int){}
    void save_override(const boost::archive::class_id_reference_type & t, int){}
    void save_override(const boost::archive::class_name_type & t, int){}
    void save_override(const boost::archive::tracking_type & t, int){}
    
public:
    ws_xml_oarchive(std::ostream & os, unsigned int flags = 0) :
        boost::archive::xml_oarchive_impl<ws_xml_oarchive>(
            os, 
            flags | boost::archive::no_header
        )
    {}
};

    template<>
    void ws_xml_oarchive::save_override(const boost::serialization::nvp< bool > & t, int);
}
}

#endif // ANSWER_WS_XML_OARCHIVE_HPP
