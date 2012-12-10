#ifndef WS_XML_IARCHIVE_HPP
#define WS_XML_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// ws_xml_iarchive.hpp

#include <ostream>
// #include <cstddef> // std::size_t
//
// #include <boost/config.hpp>
// #if defined(BOOST_NO_STDC_NAMESPACE)
// namespace std{
//     using ::size_t;
// } // namespace std
// #endif

#include <boost/type_traits/is_enum.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/algorithm/string.hpp>

#include <list>

#include <libxml++/libxml++.h>

// #include "../Utils.hh"
// #include "../Exception.hh"

class ws_xml_iarchive{

// 	unsigned int m_depth;
	std::string m_currentName;

	xmlpp::Element* _currElem;
	
	xmlpp::DomParser _parser;


		template<class Archive>
    struct load_enum_type {
        template<class T>
        static void invoke(Archive &ar, T &t){
					std::string text(ar.getNodeString());
					try{
					t = boost::lexical_cast<T>(text);
					}catch(boost::bad_lexical_cast &ex){
						std::stringstream ss;
						ss << "Failed to load enum: [" << text << "]";
// 						throw answer::WebMethodInvalidInput(ss.str());
					}
        }
    };
    template<class Archive>
    struct load_primitive {
        template<class T>
        static void invoke(Archive & ar, T & t){
					std::string text(ar.getNodeString());
					try{
					t = boost::lexical_cast<T>(text);
					}catch(boost::bad_lexical_cast &ex){
						std::stringstream ss;
						ss << "Failed to load primitive: [" << text << "]";
// 						throw answer::WebMethodInvalidInput(ss.str());
					}
        }
    };

    template<class Archive>
    struct load_only {
        template<class T>
        static void invoke(Archive & ar, const T & t){
            // make sure call is routed through the highest interface that might
            // be specialized by the user.
            boost::serialization::serialize_adl(
                ar, const_cast<T &>(t), boost::serialization::version< T >::value
            );
        }
    };
    template<class T>
    void load(T &t){
// 			int status;
				typedef
					BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
							boost::is_enum< T >,
							boost::mpl::identity<load_enum_type<ws_xml_iarchive> >,
							//else
							BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
							// if its primitive
								boost::mpl::equal_to<
									boost::serialization::implementation_level< T >,
										boost::mpl::int_<boost::serialization::primitive_type>
								>,
									boost::mpl::identity<load_primitive<ws_xml_iarchive>
								>,
								// else
						boost::mpl::identity<load_only<ws_xml_iarchive> >
            > >::type typex;
        typex::invoke(*this, t);
    }
//     #ifndef BOOST_NO_STD_WSTRING
//     void load(const std::wstring &/*ws*/){
//         m_is << "wide string types not suported in log archive";
//     }
//     #endif

		bool iterateNode(){
			
			//try to go to down to the next level
			xmlpp::NodeSet children = _currElem->find("*");
			if (!children.empty()){
				_currElem = static_cast<xmlpp::Element *>(children.front());
				return true;
			}
			
			//try to go to the next sibling, or to an ancestor's sibling
			xmlpp::Element * nextElement = _currElem;
			while (nextElement){
				xmlpp::NodeSet next = nextElement->find("following-sibling::*[1]");
				if (!next.empty()){
					_currElem = static_cast<xmlpp::Element *>(next.front());
					return true;
				}
				nextElement = nextElement->get_parent();
			}
			return false;
		}

		std::string getNodeString(){
			const xmlpp::Element *elem = static_cast<const xmlpp::Element *> (_currElem);
			const xmlpp::TextNode* text = elem->get_child_text();
			return text ? text->get_content(): "";
		}

		void load(std::string & str){
			str = getNodeString();
		}

		void load (bool & t){
			std::string text(getNodeString());
			boost::algorithm::to_lower(text);
			t = (text == "true");
		}
    
public:
    ///////////////////////////////////////////////////
    // Implement requirements for archive concept

    typedef boost::mpl::bool_<true> is_loading;
    typedef boost::mpl::bool_<false> is_saving;

    // this can be a no-op since we ignore pointer polymorphism
    template<class T>
    void register_type(T * = NULL){}

    unsigned int get_library_version(){
        return 0;
    }

		void reset_object_address(const void * /*new_address*/, const void * /*old_address*/){
// 			throw std::runtime_error("No support for pointers in archive");
		}

//     void
//     load_binary(const void */*address*/, std::size_t /*count*/){
//         m_is >> "load_binary not implemented";
//     }

    // the >> operators
    template<class T>
    ws_xml_iarchive & operator>>(T & t){
        load(t);
        return * this;
    }

//     template<class T>
//     ws_xml_iarchive & operator>>(T * t){
//         m_is << " ->";
//         if(NULL == t)
//             m_is << " null";
//         else
//             *this << * t;
//         return * this;
//     }

    template<class T, int N>
    ws_xml_iarchive & operator>>(T (&t)[N]){
        return *this >> boost::serialization::make_array(
            static_cast<T *>(&t[0]),
            N
        );
    }
    
		template<class T>
		ws_xml_iarchive & operator>>(const boost::serialization::nvp< T > & t){
			std::string itemName = t.name();
			bool isNewEntry = false;
			if (!m_collectionStack.empty() ){
// 				if (itemName == "item_version")
// 					return *this;
				if (itemName == "item"){
					if (m_collectionStack.back().newEntry){
						m_collectionStack.back().newEntry = false;
						isNewEntry = true;
					}
					itemName = m_collectionStack.back().itemName;
					if (--m_collectionStack.back().size == 0){
						m_collectionStack.pop_back();
					}
				}
			}
			if (!isNewEntry){
				iterateNode();
			}
			m_currentName = itemName;
				
			if (_currElem->get_name() != m_currentName){
// 				std::cerr << "Elements differ " << _currElem->get_name() << ',' << m_currentName  << std::endl;
				throw xmlpp::parse_error("Error parsing xml stream");
			}
			(* this) >> t.value();
			return * this;
		}

		// the & operator
		template<class T>
		ws_xml_iarchive & operator&(T & t){
			*this >> t;
			return *this;
    }

		~ws_xml_iarchive(){
		}

		struct collectionEntry{
			std::string itemName;
			size_t size;
			bool newEntry;
		};
		
		std::list<collectionEntry> m_collectionStack;

		ws_xml_iarchive(std::istream & is)/* :
			m_depth(0)*/
		{
			_parser.parse_stream(is);
			
			if (!_parser)
				throw xmlpp::parse_error("Error parsing stream");
			_currElem = _parser.get_document()->get_root_node();
		}
};

template<>
ws_xml_iarchive & ws_xml_iarchive::operator>>(const boost::serialization::nvp< boost::serialization::collection_size_type > & t);


// required by export
// BOOST_SERIALIZATION_REGISTER_ARCHIVE(ws_xml_iarchive)


#endif // WS_XML_IARCHIVE_HPP
