#ifndef WS_JSON_OARCHIVE_HPP
#define WS_JSON_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// simple_log_archive.hpp

#include <list>
#include <ostream>
#include <cstddef> // std::size_t
#include <cmath>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <boost/type_traits/is_enum.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/graph/graph_concepts.hpp>

namespace answer{
namespace archive{

class ws_json_oarchive:
    public boost::archive::detail::common_oarchive<ws_json_oarchive> {

	std::string m_currentName;
	struct collectionEntry{
		std::string itemName;
		size_t size;
		bool newEntry;
	};
	std::list<collectionEntry> m_collectionStack;

	// permit serialization system privileged access to permit
	// implementation of inline templates for maximum speed.
	friend class boost::archive::save_access;

	std::ostream & _os;
	bool _first_node;
	bool _isItem;
	bool _complex_object_begin;
	bool _complex_object_end;
	bool _wasLastEntry;
	
	template<class Archive>
	struct save_enum_type {
		template<class T>
		static void invoke(Archive &ar, const T &t){
			(ar._os) << static_cast<int>(t);
		}
	};
	template<class Archive>
	struct save_primitive {
		template<class T>
		static void invoke(Archive & ar, const T & t){
      //JSON types have some caveats, we need to detect what conversion
      // Primitive types are doing
      //TODO: this should also encode 0 starting digits with quotes
			(ar._os) << '"' << t << '"';
		}
	};

	template<class Archive>
	struct save_only {
		template<class T>
		static void invoke(Archive & ar, const T & t){
			bool first_node_tmp = ar._first_node;
			ar._first_node = true;
			ar._complex_object_begin = true;
			// make sure call is routed through the highest interface that might
			// be specialized by the user.
			boost::serialization::serialize_adl(
				ar,
				const_cast<T &>(t),
				::boost::serialization::version< T >::value
			);
			if (ar._wasLastEntry){
				ar._wasLastEntry = false;
			}else{
				ar._complex_object_end = true;
			}
			ar._first_node = first_node_tmp;
		}
	};
	
	template<class T>
	void save(const T &t){
		typedef
			BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
				boost::is_enum< T >,
				boost::mpl::identity<save_enum_type<ws_json_oarchive> >,
				//else
				BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
				// if its primitive
					boost::mpl::equal_to<
						boost::serialization::implementation_level< T >,
							boost::mpl::int_<boost::serialization::primitive_type>
					>,
          boost::mpl::identity<save_primitive<ws_json_oarchive>
					>,
					// else
			boost::mpl::identity<save_only<ws_json_oarchive> >
			> >::type typex;
		typex::invoke(*this, t);
	}
	#ifndef BOOST_NO_STD_WSTRING
	void save(const std::wstring &/*ws*/){
		_os << "wide string types not suported in log archive";
	}
	#endif

  void save(double d){
    if (std::isfinite(d)){
      _os << d;
    }else{
      _os << '"' << d << '"';
    }
  }

  void save(std::string str){
    //TODO: Moving escaping to static type resolution will speed up all the non string types which are being cast to string
    boost::replace_all(str, "\"", "\\\"");
    _os << '"' << str << '"';
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
		_os << "save_binary not implemented";
	}

	// the << operators
	template<class T>
	ws_json_oarchive & operator<<(T const & t){
			save(t);
			return * this;
	}

	template<class T>
	ws_json_oarchive & operator<<(T * const t){
			     _os << " ->";
			if(NULL == t)
					       _os << " null";
			else
					*this << * t;
			return * this;
	}
	template<class T, int N>
	ws_json_oarchive & operator<<(const T (&t)[N]){
			return *this << boost::serialization::make_array(
					static_cast<const T *>(&t[0]),
					N
			);
	}

	ws_json_oarchive &  operator<<(const boost::serialization::nvp< boost::serialization::collection_size_type > & t){
		collectionEntry aux;
		aux.itemName = m_currentName;
		aux.size = t.value();
		m_collectionStack.push_back(aux);
		return * this;
	}

	template<class T>
	ws_json_oarchive & operator<<(const boost::serialization::nvp< T > & t){
		std::string itemName (t.name());
		bool isLastEntry = false;
		bool isItem = false;
		if (!m_collectionStack.empty() ){
			if (itemName == "item_version"){
        _os << '[';
        //An empty collection
        if (m_collectionStack.back().size == 0){
          m_collectionStack.pop_back();
          _os << ']';
          _wasLastEntry = true;
        }
				return * this;
      }
			if (itemName == "item"){
				isItem = true;
				if (_complex_object_begin){ // Not a complex object, just a item
					_complex_object_begin = false;
					_complex_object_end = false;
        }
				itemName = m_collectionStack.back().itemName;
				if (--m_collectionStack.back().size == 0){
					m_collectionStack.pop_back();
					isLastEntry = true;
				}
			}
		}
		
		if (_first_node){
			_first_node = false;
		}else{
			_os << ',';
		}
		
		m_currentName = itemName;
		if (_complex_object_begin){
			_complex_object_begin = false;
			_os << '{';
		}
		if (!isItem){
			_os << '"'<< itemName << "\":";
		}
		* this << t.const_value();
		if (_complex_object_end){
			_complex_object_end = false;
			_os << '}';
		}
		if (isLastEntry){
			_os << ']';
			_wasLastEntry = true;
		}

		return * this;
	}

	// the & operator
	template<class T>
	ws_json_oarchive & operator&(const T & t){
		return * this << t;
	}
	///////////////////////////////////////////////

	~ws_json_oarchive(){
    if (_complex_object_end){
      _os << '}';
    }
	}

	ws_json_oarchive(std::ostream & os) :
		_os(os),
		_first_node(true),
		_complex_object_begin(false),
		_complex_object_end(false),
		_wasLastEntry(false)
	{
    _os.setf(std::ios::boolalpha);
	}
};

} //namespace archive
} //namespace answer

// required by export
// BOOST_SERIALIZATION_REGISTER_ARCHIVE(ws_json_oarchive)

#endif // WS_JSON_OARCHIVE_HPP
