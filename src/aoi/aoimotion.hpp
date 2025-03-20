#ifndef AOIMOTION_HPP
#define AOIMOTION_HPP

#include <Poco/Net/HTTPRequest.h>
#include <Poco/StreamCopier.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/URI.h>
#include <uv.h>
#include <vector>
#include <memory>
#include <iostream>
#include <optional>
#include <string>
#include <array>
#include <tuple>
#include <mutex>
#include "../declarations/declarations.hpp"

typedef std::pair<std::string, std::string> aoiheaders;
typedef uv_loop_t motion;
typedef uv_work_t engine;


#define DEFAULT_HEADERS {{"Content-Type", "application/json"}}


namespace AOINET {

    static const str _GET = "GET";
    static const str _POST = "POST";
    static const str _PATCH = "PATCH";
    static const str _DELETE = "DELETE";
    static const str _PUT = "PUT";
    static const str _HEAD = "HEAD";
    static const str _OPTIONS = "OPTIONS";

};

typedef struct {

    Poco::Net::HTTPResponse response;
    str responseStream;
    u16 get_status() { return response.getStatus(); }

} aoihttp;

typedef struct {

    aoihttp result;
    i32 id;
    engine worker;
    str url;
    str METHOD;
    std::vector<aoiheaders> headers;
    str body;
    bool useSSL;

    
} MotionRequest;

typedef struct {

    str METHOD;
    std::vector<aoiheaders> headers;
    str body;
    bool useSSL;


} aoibuilder;



#endif 