#ifndef WS_XML_OARCHIVE_HPP
#define WS_XML_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// simple_log_archive.hpp

#include <ostream>
#include <cstddef> // std::size_t

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <boost/type_traits/is_enum.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/optional.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/serialization/item_version_type.hpp>
// #include <boost/serialization/optional.hpp>

#include <list>

#include "../Utils.hh"

namespace answer{
namespace archive{

/////////////////////////////////////////////////////////////////////////
// log data to an output stream.  This illustrates a simpler implemenation
// of text output which is useful for getting a formatted display of
// any serializable class.  Intended to be useful as a debugging aid.
class ws_xml_oarchive:
    public boost::archive::detail::common_oarchive<ws_xml_oarchive> {

    // permit serialization system privileged access to permit
    // implementation of inline templates for maximum speed.
    friend class boost::archive::save_access;
			
    std::ostream & m_os;
//     unsigned int m_depth;
		std::string m_currentName;

		struct collectionEntry{
			std::string itemName;
			size_t size;
			bool newEntry;
		};
		std::list<collectionEntry> m_collectionStack;

    template<class Archive>
    struct save_enum_type {
        template<class T>
        static void invoke(Archive &ar, const T &t){
            (ar.m_os) << static_cast<int>(t);
        }
    };
    template<class Archive>
    struct save_primitive {
        template<class T>
        static void invoke(Archive & ar, const T & t){
            (ar.m_os) << t;
        }
    };

    template<class Archive>
    struct save_only {
        template<class T>
        static void invoke(Archive & ar, const T & t){
            // make sure call is routed through the highest interface that might
            // be specialized by the user.
            boost::serialization::serialize_adl(
                ar,
                const_cast<T &>(t),
                ::boost::serialization::version< T >::value
            );
        }
    };
		template<class T>
		void save(const T &t){
				typedef
						BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
							boost::is_enum< T >,
							boost::mpl::identity<save_enum_type<ws_xml_oarchive> >,
							//else
							BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
							// if its primitive
								boost::mpl::equal_to<
									boost::serialization::implementation_level< T >,
										boost::mpl::int_<boost::serialization::primitive_type>
								>,
									boost::mpl::identity<save_primitive<ws_xml_oarchive>
								>,
								// else
						boost::mpl::identity<save_only<ws_xml_oarchive> >
						> >::type typex;
				typex::invoke(*this, t);
		}
    #ifndef BOOST_NO_STD_WSTRING
    void save(const std::wstring &/*ws*/){
        m_os << "wide string types not suported in log archive";
    }
    #endif
    
		std::string encodeForXml( const std::string &sSrc) {
				std::ostringstream sRet;
				for ( std::string::const_iterator iter = sSrc.begin(); iter!=sSrc.end(); ++iter ) {
						unsigned char c = (unsigned char)*iter;
						switch ( c ) {
						case '&':
								sRet << "&amp;";
								break;
						case '<':
								sRet << "&lt;";
								break;
						case '>':
								sRet << "&gt;";
								break;
						case '"':
								sRet << "&quot;";
								break;
						case '\'':
								sRet << "&apos;";
								break;

						default:
								sRet << c;
						}
				}
				return sRet.str();
		}
		void save(const std::string & str){
			m_os << encodeForXml(str);
		}

		void save(const bool & value){
			m_os << (value?"true":"false");
		}
    
public:
    ///////////////////////////////////////////////////
    // Implement requirements for archive concept

    typedef boost::mpl::bool_<false> is_loading;
    typedef boost::mpl::bool_<true> is_saving;

    // this can be a no-op since we ignore pointer polymorphism
    template<class T>
    void register_type(const T * = NULL){}

    unsigned int get_library_version(){
        return 0;
    }

    void
    save_binary(const void */*address*/, std::size_t /*count*/){
        m_os << "save_binary not implemented";
    }

    // the << operators
    template<class T>
    ws_xml_oarchive & operator<<(T const & t){
        save(t);
        return * this;
    }
    template<class T>
    ws_xml_oarchive & operator<<(T * const t){
        m_os << " ->";
        if(NULL == t)
            m_os << " null";
        else
            *this << * t;
        return * this;
    }
    template<class T, int N>
    ws_xml_oarchive & operator<<(const T (&t)[N]){
        return *this << boost::serialization::make_array(
            static_cast<const T *>(&t[0]),
            N
        );
    }
    
    template<class T>
    ws_xml_oarchive & operator<<(const boost::serialization::nvp< T > & t){
			std::string itemName (t.name());
			bool isLastEntry = false;
			bool isNewEntry = false;
			if (!m_collectionStack.empty() ){
				if (itemName == "item_version")
					return *this;
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
			m_currentName = itemName;

			if (!isNewEntry){
				m_os << '<' << itemName << '>';
			}
			* this << t.const_value();
			if (!isLastEntry){
				m_os << "</" << itemName << '>';
			}
			return * this;
    }
    
		template<class T>
		ws_xml_oarchive & operator<<(const boost::serialization::nvp< boost::optional<T> > & t){
			if (!t.const_value().is_initialized()){ // Do not output
				return * this;
			}
			//Unwrap
			const boost::serialization::nvp< T > aux(t.name(), t.value().get());
			return * this << aux;
		}

		// the & operator
		template<class T>
		ws_xml_oarchive & operator&(const T & t){
			return * this << t;
		}
		
    ws_xml_oarchive(std::ostream & os) :
        m_os(os)
    {}
};

template<>
ws_xml_oarchive & ws_xml_oarchive::operator<<(const boost::serialization::nvp< boost::serialization::collection_size_type > & t);

}//namespace archive
}//namespace answer

// required by export
// BOOST_SERIALIZATION_REGISTER_ARCHIVE(ws_xml_oarchive)

#endif // WS_XML_OARCHIVE_HPP
