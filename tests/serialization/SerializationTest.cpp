#define BOOST_TEST_MODULE SerializationTest
#include <boost/test/included/unit_test.hpp>

#include "answer/archive/ws_xml_oarchive.hpp"
#include "answer/archive/ws_xml_iarchive.hpp"

#include "answer/archive/ws_json_oarchive.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <map>

using namespace std;

struct complexData
{
  std::multiset<double> set_collection;
  std::vector<double> vector_collection;
  std::list<std::vector<int>> nested_collection;
  string str;

  bool operator== (const complexData &t)
  {
    return set_collection == t.set_collection &&
            vector_collection == t.vector_collection &&
//             nested_collection == t.nested_collection &&
            str == t.str;
    
  }

  template<class Archive>
  void serialize(Archive &ar, const unsigned int /*version*/)
  {
    ar & BOOST_SERIALIZATION_NVP(set_collection);
    ar & BOOST_SERIALIZATION_NVP(vector_collection);
//     ar & BOOST_SERIALIZATION_NVP(nested_collection);
    ar & BOOST_SERIALIZATION_NVP(str);
  }
};

BOOST_AUTO_TEST_CASE(serialization)
{
  complexData data;
  data.set_collection = {3.12, 400, -1234, 3e4, 3};
  data.vector_collection = {3.12, 400, -1234, 3e4, 3};
  // TODO: add support for nested collections
  //   data.nested_collection = {{3, 12, 400}, {-1234, 3, 3}};
  data.str = "string";

  stringstream ss;
  {
    answer::archive::ws_xml_oarchive oarchive(ss);
    oarchive << boost::serialization::make_nvp("test", data);
  }
  complexData data_aux;
  {
    answer::archive::ws_xml_iarchive iarchive(ss);
    iarchive >> boost::serialization::make_nvp("test", data_aux);
  }
  
  BOOST_CHECK(data == data_aux);
}

BOOST_AUTO_TEST_CASE(xml_empty)
{
  stringstream ss;
  {
    string empty;
    answer::archive::ws_xml_oarchive oarchive(ss);
    oarchive << boost::serialization::make_nvp("test", empty);
  }
  
  const auto xml = boost::trim_copy(ss.str());

  BOOST_CHECK( xml == "<test></test>");
}

BOOST_AUTO_TEST_CASE(json_empty)
{

  stringstream ss;
  {
    string empty;
    answer::archive::ws_json_oarchive oarchive(ss);
    oarchive << boost::serialization::make_nvp("test", empty);
  }
  
  const auto json = boost::trim_copy(ss.str());

  BOOST_CHECK( json == "\"test\":\"\"");
}