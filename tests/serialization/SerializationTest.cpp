#define BOOST_TEST_MODULE SerializationTest
#include <boost/test/included/unit_test.hpp>

#include "answer/archive/ws_xml_oarchive.hpp"
#include "answer/archive/ws_xml_iarchive.hpp"
#include <boost/optional.hpp>
#include <boost/serialization/set.hpp>
#include <map>


using namespace std;

struct testInput
{
  std::set<int> test;

  bool operator== (const testInput &t)
  {
    return test == t.test;
  }

  template<class Archive>
  void serialize(Archive &ar, const unsigned int /*version*/)
  {
    ar &BOOST_SERIALIZATION_NVP(test);
  }
};

BOOST_AUTO_TEST_CASE(serialization)
{
  testInput testIntSet1;
  testIntSet1.test.insert(3);
  testIntSet1.test.insert(400);
  testIntSet1.test.insert(-1234);
  testIntSet1.test.insert(3e4);
  testIntSet1.test.insert(3);

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
