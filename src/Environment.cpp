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

const std::vector<Response::Header>& Response::headers() const{
  return _headers;
}

void Response::header(const std::string &key, const std::string &value){
  _headers.emplace_back(key, value);
}

}