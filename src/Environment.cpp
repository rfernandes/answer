#include "answer/Environment.hh"

#include <boost/filesystem.hpp>

using namespace std;

namespace answer
{

const string &Response::body() const
{
  return _body;
}

void Response::body(const string &val)
{
  _body = val;
}

const string &Response::contentType() const
{
  return _contentType;
}

void Response::contentType(const string &val)
{
  _contentType = val;
}

const vector<Response::Header> &Response::headers() const
{
  return _headers;
}

void Response::header(const string &key, const string &value)
{
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
void Request::body(const string &value)
{
  _body = value;
}
const string &Request::body() const
{
  return _body;
}

namespace{
  
}// anonymous namespace

OperationInfo::OperationInfo(string url):
  _url(std::move(url))
{
  //TODO: Determine if the url is in query or SEF/REST mode
  boost::filesystem::path path(_url);
  _service = path.parent_path().string();
  _service = _service.substr(_service.find("/", 1) + 1);
  _operation = path.filename().string(); 
}

const string &OperationInfo::url() const
{
  return _url;
}
const string &OperationInfo::service() const
{
  return _service;
}
const string &OperationInfo::operation() const
{
  return _operation;
}

const map<Response::Status, string> Response::statusText =
{
  {Status::OK, "OK"},
  {Status::ACCEPTED, "Accepted"},
  {Status::FOUND, "Found"},
  {Status::FORBIDDEN, "Forbiden"},
  {Status::NOT_FOUND, "Not Found"},
  {Status::INTERNAL_SERVER_ERROR, "Internal Server Error"}
};

}
