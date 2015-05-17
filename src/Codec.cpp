#include "answer/Codec.hh"

namespace answer
{
namespace codec
{
bool GenericEncoder(std::ostream &out, const std::string &mimeType, const std::string &operationName)
{
  if (mimeType == "application/json")
  {
    out << "null";
    return true;
  }

  if (mimeType == "application/xml" || mimeType == "*/*")
  {
    answer::archive::ws_xml_oarchive outA(out);
    outA << boost::serialization::make_nvp(operationName.c_str(), "");
    return true;
  }
  return false;
}

bool GenericDecoder(std::ostream &in, const std::string &mimeType, const std::string &operationName)
{
  if (mimeType == "application/json")
  {
    return true;
  }

  if (mimeType == "application/xml" || mimeType == "*/*")
  {
    return true;
  }
  return false;
}

}
}
