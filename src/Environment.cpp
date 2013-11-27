#include "answer/Environment.hh"

using namespace std;

namespace answer{

const string &Response::body() const{
  return _body;
}

void Response::body(const string &val){
  _body = val;
}

const string &Response::contentType() const{
  return _contentType;
}

void Response::contentType(const string &val){
  _contentType = val;
}

const vector<Response::Header>& Response::headers() const{
  return _headers;
}

void Response::header(const string &key, const string &value){
  _headers.emplace_back(key, value);
}

void Response::status(Response::Status val)
{
  _status = val;
}
Response::Status Response::status() const
{
  return _status;
}

const map<Response::Status, string> Response::statusText = {
  {Status::OK, "OK"},
  {Status::ACCEPTED, "Accepted"},
  {Status::FOUND, "Found"},
  {Status::FORBIDDEN, "Forbiden"},
  {Status::NOT_FOUND, "Not Found"},
  {Status::INTERNAL_SERVER_ERROR, "Internal Server Error"}
};

}