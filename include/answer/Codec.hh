#ifndef _CODEC_HH_
#define _CODEC_HH_

#include <string>
#include <iostream>
#include "archive/ws_json_oarchive.hpp"
#include "archive/ws_xml_oarchive.hpp"
#include "archive/ws_xml_iarchive.hpp"
#include "Exception.hh"
#include "Context.hh"

namespace answer
{
namespace codec
{
template<typename T>
bool Codec(std::ostream &, const std::string &, const T &)
{
  return false;
}

template<typename T>
bool GenericEncoder(std::ostream &out, const std::string &mimeType, const std::string &operationName, const T &data)
{
  if (mimeType == "application/json")
  {
    archive::ws_json_oarchive outA(out);
    outA << data;
    return true;
  }
  if (mimeType == "application/xml" || mimeType == "*/*")
  {
    archive::ws_xml_oarchive outA(out);
    outA << boost::serialization::make_nvp(operationName.c_str(), data);
    return true;
  }
  return false;
}

//A conformant request only operation must still return something depending on mimeType
bool GenericEncoder(std::ostream &out, const std::string &mimeType, const std::string &operationName);


template<typename T>
bool GenericDecoder(std::istream &in, const std::string &contentType, const std::string &operationName, const T &data)
{
  if (contentType == "application/json")
  {
//     archive::ws_json_oarchive outA(in);
//     data >> outA;
    return false;
  }
  if (contentType == "application/xml" || contentType == "*/*")
  {
    archive::ws_xml_iarchive archive(in);
    archive >> boost::serialization::make_nvp(operationName.c_str(), data);
    return true;
  }
  return false;
}

bool GenericDecoder(std::ostream &out, const std::string &contentType, const std::string &operationName);

template<typename T>
bool ResponseWrapper(std::ostream &out, const std::string &response, const std::string & /*mimeType*/, const WebMethodException *ex)
{
  if (ex)
  {
    out << ex->what();
  }
  else
  {
    out << response;
  }
  return true;
}

}
}

#define ANSWER_RESPONSE_WRAPPER(WrapperFunction)\
  namespace answer{\
  namespace codec{\
  template<>\
  bool ResponseWrapper<void>( std::ostream& out, const std::string& response, const std::string& mimeType, const answer::WebMethodException* ex){\
    return WrapperFunction(out, response, mimeType, ex);\
  }\
  }\
  }\
   
#endif // _CODEC_HH_
