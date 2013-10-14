#include "answer/Environment.hh"

using namespace std;

namespace answer{

const string &Response::body() const{
  return _body;
}

void Response::body(const string &body){
  _body = body;
}

const string &Response::contentType() const{
  return _contentType;
}

void Response::contentType(const string &contentType){
  _contentType = contentType;
}

const vector<Response::Header>& Response::headers() const{
  return _headers;
}

void Response::header(const string &key, const string &value){
  _headers.emplace_back(key, value);
}

void Response::status(Response::Status status)
{
  _status = status;
}
Response::Status Response::status() const
{
  return _status;
}

const map<Response::Status, string> Response::statusText = {
  {Status::OK_, "OK"},
  {Status::ACCEPTED, "Accepted"},
  {Status::NOT_FOUND, "Not Found"}
};

}