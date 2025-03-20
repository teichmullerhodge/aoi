#ifndef AOIMOTION_HPP
#define AOIMOTION_HPP

#include "../declarations/declarations.hpp"
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <tuple>
#include <uv.h>
#include <vector>
/// @brief alias for std::pair<std::string, std::string> since every headers is
/// a Key/Value of strings.
typedef std::pair<std::string, std::string> aoiheaders;
/// @brief alias for the uv_loop_t since our types aren't declared in snake case
/// manner.
typedef uv_loop_t motion;
/// @brief alias for the uv_work_t since our types aren't declared in snake case
/// manner. Together with the motion this creates the "motion engine" type.
/// easter-egg of the PS2.
typedef uv_work_t engine;

#define DEFAULT_HEADERS                                                        \
  {                                                                            \
    { "Content-Type", "application/json" }                                     \
  }

/// @brief Namespace to define constants like "GET", "POST", etc.
namespace AOINET {

static const str _GET = "GET";
static const str _POST = "POST";
static const str _PATCH = "PATCH";
static const str _DELETE = "DELETE";
static const str _PUT = "PUT";
static const str _HEAD = "HEAD";
static const str _OPTIONS = "OPTIONS";

}; // namespace AOINET

/// @brief This is the base structure that is returned
/// in the requests using aoi. It has 2 variables,
/// response that is a Poco::Net::HTTPResponse class
/// and responseStream that is the str of the response returned
typedef struct {

  Poco::Net::HTTPResponse response;
  str responseStream;
  u16 get_status() { return response.getStatus(); }

} aoihttp;

/// @brief the aoibuilder is the base configuration
/// for every request. Is where is defined the
/// METHOD ("GET", "POST", etc), the headers, the body and
/// a boolean flag to use SSL or discard it when making HTTP requests.
typedef struct {

  str METHOD;
  std::vector<aoiheaders> headers;
  str body;
  bool useSSL;

} aoibuilder;

#define DEFAULT_BUILDER                                                        \
  { AOINET::_GET, DEFAULT_HEADERS, "", true }

/// @brief the aoidata is a struct used to operate
/// in the async methods on the aoiclass. It encodes
/// the url of the requests, the aoibuilder
/// the aoihttp response, the worker and a callback
/// to be used when the function is finished.
typedef struct {

  str url;
  aoibuilder builder;
  aoihttp response;
  engine worker;
  std::function<void(aoihttp)> callback;
} aoidata;

#define then []
#endif