#include "../declarations/declarations.hpp"
#include "aoimotion.hpp"
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <array>
#include <assert.h>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <uv.h>
#include <vector>

/// @brief class to wrap the callbacks used when working with
/// libuv. This class should not be instantiated.
/// the callbacks for the async executions are denoted:
/// callback_(function_name)(engine* req, s32 status)
/// engine* is an alias for uv_work_t*
class aoicallback {
public:
  aoicallback() {}
  ~aoicallback() {}

  /// @brief callback to the perform_async method. It sets the callback
  /// of the request after it's done, if any. After this it deletes the
  /// aoidata* request
  static void callback_perform_async(engine *req, s32 status) {
    aoidata *request = static_cast<aoidata *>(req->data);
    if (status < 0) {
      std::cerr << "Error in [callback_perform_async]" << "\n";
      delete request;
    }
    if (request->callback) {
      request->callback(request->response);
    }
    delete request;
  }
};

/// @brief A class to wrap methods to perform blocking
/// and non-blocking http requests.
class aoi {

private:
  /// @brief Set the headers to the request.
  /// @param req a reference to Poco::Net::HTTPRequest
  /// @param headers an vector of pairs of std::string
  static void set_headers(Poco::Net::HTTPRequest &req,
                          std::vector<aoiheaders> headers) {

    for (const auto &h : headers) {
      req.set(h.first, h.second);
    }
  }

public:
  /// @brief checks the status code of the request.
  /// @param status unsigned integer that holds the http status code.
  /// @return true if the status is in the range [200, 299]
  static bool status_ok(u16 status) { return (status - 200) < 100; }

  /// @brief This method performs a blocking request.
  /// @param url the url desired
  /// @param builder the HTTP/Client configuration structure
  /// @return returns an aoihttp structure.
  static aoihttp perform(str url,
                         aoibuilder builder = {AOINET::_GET, DEFAULT_HEADERS,
                                               "", true}) {
    try {
      Poco::URI uri(url);
      std::unique_ptr<Poco::Net::HTTPClientSession> session;
      if (builder.useSSL) {
        session = std::make_unique<Poco::Net::HTTPSClientSession>(
            uri.getHost(), uri.getPort());
      } else {
        session = std::make_unique<Poco::Net::HTTPClientSession>(uri.getHost(),
                                                                 uri.getPort());
      }

      Poco::Net::HTTPRequest request(builder.METHOD, uri.getPathAndQuery(),
                                     Poco::Net::HTTPMessage::HTTP_1_1);
      request.set("Host", uri.getHost());
      set_headers(request, builder.headers);

      if (builder.METHOD == AOINET::_POST || builder.METHOD == AOINET::_PUT) {
        if (!builder.body.empty()) {
          std::ostream &os = session->sendRequest(request);
          os << builder.body;
        }
      }

      session->sendRequest(request);
      std::ostringstream oss;
      Poco::Net::HTTPResponse response;
      std::istream &rs = session->receiveResponse(response);
      str responseText;
      Poco::StreamCopier::copyToString(rs, responseText);
      aoihttp r = {response, responseText};
      return r;

    } catch (const Poco::Exception &e) {
      std::cerr << "Exception: " << e.displayText() << '\n';
      return aoihttp{};
    }
  }
  /// @brief Performs an async / non-blocking request.
  /// @param url the desired url
  /// @param builder the HTTP/Client configuration structure
  /// @param callback a callback to be called after the request is done.
  static void async_perform(
      str url, aoibuilder builder = {AOINET::_GET, DEFAULT_HEADERS, "", true},
      std::function<void(aoihttp)> callback = [](aoihttp h) { (void)h; }) {
    aoidata *data = new aoidata{url, builder, {}, {}, callback};
    data->worker.data = data;
    uv_queue_work(uv_default_loop(), &data->worker, aoi::async_perform_engine,
                  aoicallback::callback_perform_async);
  }

private:
  /// @brief Performs an async request, this method is private and
  /// is used internally to operate with libuv uv_work_t. Use
  /// aoi::async_perform instead.
  /// @param worker engine* is a alias for uv_work_t *
  static void async_perform_engine(engine *worker) {
    aoidata *data = static_cast<aoidata *>(worker->data);
    try {

      Poco::URI uri(data->url);
      std::unique_ptr<Poco::Net::HTTPClientSession> session;
      if (data->builder.useSSL) {
        session = std::make_unique<Poco::Net::HTTPSClientSession>(
            uri.getHost(), uri.getPort());
      } else {
        session = std::make_unique<Poco::Net::HTTPClientSession>(uri.getHost(),
                                                                 uri.getPort());
      }

      Poco::Net::HTTPRequest request(data->builder.METHOD,
                                     uri.getPathAndQuery(),
                                     Poco::Net::HTTPMessage::HTTP_1_1);
      request.set("Host", uri.getHost());
      aoi::set_headers(request, data->builder.headers);
      if (data->builder.METHOD == AOINET::_POST ||
          data->builder.METHOD == AOINET::_PUT) {
        if (!data->builder.body.empty()) {
          std::ostream &os = session->sendRequest(request);
          os << data->builder.body;
        }
      }

      session->sendRequest(request);
      Poco::Net::HTTPResponse response;
      std::istream &rs = session->receiveResponse(response);
      str responseText;
      Poco::StreamCopier::copyToString(rs, responseText);
      data->response = {response, responseText};

    } catch (const Poco::Exception &e) {
      std::cerr << "Exception: " << e.displayText() << "\n";
    }
  }
};
