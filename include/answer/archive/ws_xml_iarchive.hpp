#ifndef ANSWER_WS_XML_IARCHIVE_HPP
#define ANSWER_WS_XML_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <sstream>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/concept_check.hpp>
#include <boost/archive/xml_archive_exception.hpp>
#include <boost/archive/impl/basic_xml_grammar.hpp>
#include <boost/serialization/detail/stack_constructor.hpp>

#include <cstring>


namespace answer{
namespace archive{

struct archive_end_exception: public std::runtime_error{
	archive_end_exception(const std::string& str): runtime_error(str){}
};
	
using namespace boost::archive;

class ws_xml_iarchive : 
	public xml_iarchive_impl<ws_xml_iarchive>
{
	typedef xml_iarchive_impl<ws_xml_iarchive> base;
//     bool closeCollectionExpected;
    bool optionalNotFound;
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
	friend class detail::interface_iarchive<ws_xml_iarchive>;
	friend class basic_xml_iarchive<ws_xml_iarchive>;
	friend class xml_iarchive_impl<ws_xml_iarchive>;
	friend class load_access;
protected:
#endif

	std::string m_currentName;
	std::streampos m_lastPos;
	struct collectionEntry{
		std::string itemName;
		boost::serialization::collection_size_type* size;
		bool starting;
	};
	std::list<collectionEntry> m_collectionStack;

	//We overload load_start to allow tag name checking
	void
	load_start(const char *name, bool check_tag = false){
		// if there's no name
		if(NULL == name)
				return;
		bool result = this->This()->gimpl->parse_start_tag(this->This()->get_is());
		if(true != result){
		boost::serialization::throw_exception(
				archive_exception(archive_exception::input_stream_error)
			);
		}
		// double check that the tag matches what is expected - useful for debug
		if(0 != name[this->This()->gimpl->rv.object_name.size()]
		|| ! std::equal(
				this->This()->gimpl->rv.object_name.begin(),
				this->This()->gimpl->rv.object_name.end(),
			name
			)
		){
			boost::serialization::throw_exception(
				xml_archive_exception(
					xml_archive_exception::xml_archive_tag_mismatch,
					name
				)
			);
		}
	}
	
	// Anything not an attribute - see below - should be a name value
	// pair and be processed here
	template<class T>
	void load_override(
		#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
		const
		#endif
		boost::serialization::nvp< T > & t,
		int
	){
		
		std::string itemName = t.name();
		std::cerr << ":" << itemName << " [" << char(is.peek()) << "] [" << int(is.tellg())<< ']' <<std::endl;
		m_lastPos = is.tellg();

		m_currentName = itemName;

		if (!m_collectionStack.empty() ){
			
			if (t.name() == "item_version"){
				return; //Ignore item_version loading from default collection serialization implementations
			}

			if (itemName == "item"){ //Iterating the current collection loop
				itemName = m_collectionStack.back().itemName;
				
				// If starting a collection the stream is at the value, so we skip the load_start
				if (m_collectionStack.back().starting){
					is.seekg(is.tellg() - m_collectionStack.back().itemName.size() - 2);
					m_collectionStack.back().starting = false;
				}
				//increase the count (it will be reset and removed on parse fail == collection end
				++(*m_collectionStack.back().size);
				try{
					this->This()->load_start(itemName.c_str(), true);
				}/*catch (boost::archive::xml_archive_exception &ex){
					//This condition is the end of a collection
					is.seekg(m_lastPos - itemName.size() - 3);
					//End the collection iteration
					(*m_collectionStack.back().size) = 0;
					m_collectionStack.pop_back();
					throw answer::archive::archive_end_exception("Ending collection");
				}*/catch (boost::archive::archive_exception &ex){
					//This condition is the end of a collection
					//This is perhaps too generic but we are ending a collection
					// in this particular exception stream end was also read
					// retrace a bit and signal a collection end
					is.seekg(m_lastPos - itemName.size() - 3);
					(*m_collectionStack.back().size) = 0;
					m_collectionStack.pop_back();
					throw answer::archive::archive_end_exception("Ending collection");
				}
				boost::archive::load(* this, t.value());
				this->This()->load_end(itemName.c_str());
				return;
			}
		}
		//Normal load
		base::load_override(t, 0);
	}
	
	template<class T>
	void load_override(
		#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
		const
		#endif
		boost::serialization::nvp< boost::optional<T> > & t,
		int
	){
		m_lastPos = is.tellg();
		std::string itemName = t.name();
		
		std::cerr << ":(O):" << t.name() << " [" << char(is.peek()) << "] [" << int(is.tellg())<< ']' <<std::endl;
		try{
			this->This()->load_start(t.name(), true);
		}/*catch (boost::archive::xml_archive_exception &ex){
			// This optional is not present
			//  retrace the stream and proceed
			t.value().reset();
			is.seekg(m_lastPos );
			return;
		}*/catch (boost::archive::archive_exception &ex){
			// This is again perhaps too generic (And in this case
			//   could be merged with the above catch)
			t.value().reset();
			is.seekg(m_lastPos );
			return;
		}
		
		is.seekg(m_lastPos );
		boost::serialization::detail::stack_construct<ws_xml_iarchive, T> aux(*this, 0);
		load_override(boost::serialization::make_nvp(t.name(), aux.reference()), 0);
		t.value().reset(aux.reference());
	}
	
	void load_override(
			#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
			const
			#endif
			boost::serialization::nvp< boost::serialization::collection_size_type > & t,
			int
	){
		//Starting a collection
		t.value() = 1; //We will increment this in the loop (keep the addr in the collectionStack)
		collectionEntry entry;
		entry.itemName = m_currentName;
		entry.size = &(t.value());
		entry.starting = true;
		m_collectionStack.push_back(entry);
	}

	// specific overrides for attributes - not name value pairs so we
	// want to trap them before the above "fall through"
	// since we don't want to see these in the output - make them no-ops.
	void load_override(boost::archive::object_id_type & t, int){}
	void load_override(boost::archive::object_reference_type & t, int){}
	void load_override(boost::archive::version_type & t, int){}
	void load_override(boost::archive::class_id_type & t, int){}
	void load_override(boost::archive::class_id_optional_type & t, int){}
	void load_override(boost::archive::class_id_reference_type & t, int){}
	void load_override(boost::archive::class_name_type & t, int){}
	void load_override(boost::archive::tracking_type & t, int){}
public:
	ws_xml_iarchive(std::istream & is, unsigned int flags = 0) :
		xml_iarchive_impl<ws_xml_iarchive>(is, flags| boost::archive::no_header ),
// 		closeCollectionExpected(false),
		optionalNotFound(false)
	{}
	~ws_xml_iarchive(){}
};

} // archive
} // answer

#include <boost/serialization/collections_load_imp.hpp>
// #include "answer/archive/ws_xml_iarchive.hpp"

//Provide partial template specializations for ws_xml_iarchives;
// This allows us to cap the number items at the correct entry
// instead of iterating one final time (which would at best 
// result in an empty entry)
namespace boost{
namespace serialization {
namespace stl {
	

// sequential container input
template<class Container>
struct archive_input_seq<answer::archive::ws_xml_iarchive, Container>
{
    inline BOOST_DEDUCED_TYPENAME Container::iterator
    operator()(
        answer::archive::ws_xml_iarchive &ar, 
        Container &s, 
        const unsigned int v,
        BOOST_DEDUCED_TYPENAME Container::iterator hint
    ){
			typedef BOOST_DEDUCED_TYPENAME Container::value_type type;
			detail::stack_construct<answer::archive::ws_xml_iarchive, type> t(ar, v);
			// borland fails silently w/o full namespace
			try{
				ar >> boost::serialization::make_nvp("item", t.reference());
			}catch (answer::archive::archive_end_exception &ex){
				return hint;
			}
			s.push_back(t.reference());
			ar.reset_object_address(& s.back() , & t.reference());
			return hint;
    }
};

// map input
template<class Container>
struct archive_input_map<answer::archive::ws_xml_iarchive, Container>
{
    inline BOOST_DEDUCED_TYPENAME Container::iterator
    operator()(
        answer::archive::ws_xml_iarchive &ar,
        Container &s,
        const unsigned int v,
        BOOST_DEDUCED_TYPENAME Container::iterator hint
    ){
        typedef BOOST_DEDUCED_TYPENAME Container::value_type type;
        detail::stack_construct<answer::archive::ws_xml_iarchive, type> t(ar, v);
        // borland fails silently w/o full namespace
				try{
					ar >> boost::serialization::make_nvp("item", t.reference());
				}catch (answer::archive::archive_end_exception &ex){
					return hint;
				}
        BOOST_DEDUCED_TYPENAME Container::iterator result =
            s.insert(hint, t.reference());
        // note: the following presumes that the map::value_type was NOT tracked
        // in the archive.  This is the usual case, but here there is no way
        // to determine that.
        ar.reset_object_address(
            & (result->second),
            & t.reference().second
        );
        return result;
    }
};

template<class Container>
struct archive_input_set<answer::archive::ws_xml_iarchive, Container>
{
    inline BOOST_DEDUCED_TYPENAME Container::iterator
    operator()(
        answer::archive::ws_xml_iarchive &ar, 
        Container &s, 
        const unsigned int v,
        BOOST_DEDUCED_TYPENAME Container::iterator hint
    ){
			typedef BOOST_DEDUCED_TYPENAME Container::value_type type;
			detail::stack_construct<answer::archive::ws_xml_iarchive, type> t(ar, v);
			// borland fails silently w/o full namespace
			try{
				ar >> boost::serialization::make_nvp("item", t.reference());
			}catch (answer::archive::archive_end_exception &ex){
				return hint;
			}
			BOOST_DEDUCED_TYPENAME Container::iterator result = 
					s.insert(hint, t.reference());
			ar.reset_object_address(& (* result), & t.reference());
			return result;
    }
};

} // namespace stl 
} // namespace serialization
} // namespace boost


#endif // ANSWER_WS_XML_IARCHIVE_HPP
