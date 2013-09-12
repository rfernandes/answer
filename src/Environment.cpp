#include "answer/Environment.hh"


namespace answer{

const std::string &Response::body() const{
  return _body;
}

void Response::body(const std::string &body){
  _body = body;
}

const std::string &Response::contentType() const{
  return _contentType;
}

void Response::contentType(const std::string &contentType){
  _contentType = contentType;
}

const std::string &Response::header(const std::string &key) const{
  throw std::runtime_error("header unimplemented");
}

void Response::header(const std::string &key, const std::string &value){
  throw std::runtime_error("header unimplemented");
}

}