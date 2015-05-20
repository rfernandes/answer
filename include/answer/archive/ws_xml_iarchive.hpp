#ifndef ANSWER_WS_XML_IARCHIVE_HPP
#define ANSWER_WS_XML_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <sstream>
#include <list>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/concept_check.hpp>
#include <boost/archive/xml_archive_exception.hpp>
#include <boost/archive/impl/basic_xml_grammar.hpp>
#include <boost/serialization/detail/stack_constructor.hpp>

#include <memory>
#include <sstream>
#include <cstring>

namespace answer
{
namespace archive
{

struct archive_end_exception: public std::runtime_error
{
  archive_end_exception(const std::string &str): runtime_error(str) {}
};

struct starttag_closed_exception: public std::runtime_error
{
  starttag_closed_exception(const std::string &str): runtime_error(str) {}
};

using namespace boost::archive;

class ws_xml_iarchive :
  public xml_iarchive_impl<ws_xml_iarchive>
{
  typedef xml_iarchive_impl<ws_xml_iarchive> base;
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
  
  std::stringstream m_collectionStream;
  std::unique_ptr<ws_xml_iarchive> m_collection;
  unsigned m_collectionSize;
  bool m_collectionStart, m_collectionEnd;
  
/* 
 * Collection loading poses an interesting problem due to the xml schema we are trying
 * to enforce. Unlike xml_iachive, we are not using collection_size elements, so we
 * have to opted to overload collection_size and create an nested ws_xml_iarchive with
 * the read items. This makes the code much cleaner as it avoids having to overload
 * all the serialization loads for the various stl containers
 */

  //We overload load_start to allow tag name checking
  bool
  load_start(const char *name, bool check_tag = false)
  { 
    std::streampos beginPos = is.tellg();

    // if there's no name
    if (!name)
      return true;
    bool result = this->This()->gimpl->parse_start_tag(this->This()->get_is());
    if (!result)
    {
      if (check_tag)
      {
        //TODO: replace this with proper EBNF grammar
        std::streampos failedPos = is.tellg();
        is.seekg(beginPos);
        std::string seekClosedTag = "<";
        seekClosedTag.append(name).append("/>");
        std::string::const_iterator it = seekClosedTag.begin();
        while (is.good() && it != seekClosedTag.end() && is.get() == *it)
        {
          ++it;
        }
        if (it == seekClosedTag.end())
        {
          return false;
        }
        is.seekg(failedPos);
      }
      return false;
    }
    // double check that the tag matches what is expected - useful for debug
    if (0 != name[this->This()->gimpl->rv.object_name.size()]
        || ! std::equal(
          this->This()->gimpl->rv.object_name.begin(),
          this->This()->gimpl->rv.object_name.end(),
          name
        )
       )
    {
      return false;
    }
    return true;
  }

  // Anything not an attribute - see below - should be a name value
  // pair and be processed here
  template<class T>
  void load_override(
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    const
#endif
    boost::serialization::nvp< T > &t,
    int
  )
  { 
    if (m_collectionSize)
    {
      // Delegate to inner collection
      m_collection->load_override(t,0);
      
      if (!--m_collectionSize)
      {
        m_collection = nullptr;
      }
      return;
    }
    
    std::streampos lastPos = is.tellg();
    m_currentName = t.name();

//     std::cerr << m_currentName << " [" << char(is.peek()) << "] [" << int(is.tellg())<< ']' <<std::endl;

    if (!m_collectionStart)
    {
      bool startLoaded = this->This()->load_start(t.name(), true);
      if (!startLoaded)
      {
        // Retrace a bit and signal a collection end
        is.seekg(lastPos - std::streampos(m_currentName.size()) - 3);
        m_collectionEnd = true;
        return;
      }
    }
    else
    {
      m_collectionStart = false;
    }
    boost::archive::load(* this->This(), t.value());
    this->This()->load_end(t.name());
  }

  void load_override(
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    const
#endif
    boost::serialization::nvp< boost::serialization::collection_size_type > &t,
    int
  )
  {
    std::string itemName = m_currentName;
    
    std::string cap;
    unsigned counter = 0;
    m_collectionStart = true;
    m_collectionEnd = false;
    m_collectionStream << "<item_version>0</item_version>";
    while (true)
    {
      *this >> boost::serialization::make_nvp(itemName.c_str(), cap);
      if (m_collectionEnd)
        break;
      m_collectionStream << "<item>" << cap << "</item>";
      counter++;
    }
    t.value() = counter;
    
    m_collectionSize = counter + 1; // +1 as the same loop sequence will parse item_version
    m_collection = std::make_unique<ws_xml_iarchive>(m_collectionStream);
  }

  // specific overrides for attributes - not name value pairs so we
  // want to trap them before the above "fall through"
  // since we don't want to see these in the output - make them no-ops.
  void load_override(boost::archive::object_id_type &, int) {}
  void load_override(boost::archive::object_reference_type &, int) {}
  void load_override(boost::archive::version_type &, int) {}
  void load_override(boost::archive::class_id_type &, int) {}
  void load_override(boost::archive::class_id_optional_type &, int) {}
  void load_override(boost::archive::class_id_reference_type &, int) {}
  void load_override(boost::archive::class_name_type &, int) {}
  void load_override(boost::archive::tracking_type &, int) {}

public:
  ws_xml_iarchive(std::istream &is_, unsigned int flags = 0) :
    xml_iarchive_impl<ws_xml_iarchive>(is_, flags | boost::archive::no_header),
    m_collectionSize(0),
    m_collectionStart(false),
    m_collectionEnd(false)
  {
    is_.setf(std::ios::boolalpha);
  }
  ~ws_xml_iarchive() {}
};

} // archive
} // answer


#endif // ANSWER_WS_XML_IARCHIVE_HPP
