#define BOOST_TEST_MODULE CodecTest
#include <boost/test/included/unit_test.hpp>

#include "answer/archive/ws_xml_oarchive.hpp"
#include "answer/archive/ws_xml_iarchive.hpp"

#include "answer/archive/ws_json_oarchive.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include <boost/serialization/set.hpp>
#include <map>


using namespace std;

struct testInput
{
  std::set<double> test;

  bool operator== (const testInput &t)
  {
    return test == t.test;
  }

  template<class Archive>
  void serialize(Archive &ar, const unsigned int /*version*/)
  {
    ar & BOOST_SERIALIZATION_NVP(test);
  }
};



BOOST_AUTO_TEST_CASE(serialization)
{
  testInput testIntSet1;
  testIntSet1.test = {3.12, 400, -1234, 3e4, 3};

  stringstream ss;
  {
    answer::archive::ws_xml_oarchive oarchive(ss);
    oarchive << boost::serialization::make_nvp("test", testIntSet1);
  }
  testInput testIntSet2;
  {
    answer::archive::ws_xml_iarchive iarchive(ss);
    iarchive >> boost::serialization::make_nvp("test", testIntSet2);
  }

  BOOST_CHECK(testIntSet1 == testIntSet2);
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