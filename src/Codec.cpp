#include "answer/Codec.hh"

namespace answer
{
namespace codec
{
bool GenericCodec(std::ostream &out, const std::string &mimeType, const std::string &operationName)
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
bool Codec(std::ostream &, const std::string &)
{
  return false;
}
}
}
