#include "../declarations/declarations.hpp"
#include "aoimotion.hpp"
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <iostream>
#include <memory>
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
      bool requestSend = false;
      if (builder.METHOD == AOINET::_POST || builder.METHOD == AOINET::_PUT ||
          builder.METHOD == AOINET::_PATCH) {
        request.setContentLength(builder.body.length());
        if (!builder.body.empty()) {
          std::ostream &os = session->sendRequest(request);
          os << builder.body;
          requestSend = true;
        }
      }
      if (!requestSend) {

        session->sendRequest(request);
      }
      std::ostringstream oss;
      Poco::Net::HTTPResponse response;
      std::istream &rs = session->receiveResponse(response);
      str responseText;
      Poco::StreamCopier::copyToString(rs, responseText);
      aoihttp r = {response, responseText};
      return r;

    } catch (const Poco::Exception &e) {
      std::cerr << "Exception: " << e.displayText() << '\n';
      Poco::Net::HTTPResponse resp;
      resp.setStatus("0"); // Ugly. But without this the return would be 200.
      return aoihttp{resp, {}};
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

  /// @brief Makes multiples requests in a batch in a non-blocking way
  /// @param urls a vector of urls
  /// @param builders a vector of builders
  /// @param callbacks a vector of callbacks
  /// @param loop a motion* that is defined as uv_default_loop()
  static void
  async_perform_all(std::vector<str> urls, std::vector<aoibuilder> builders,
                    std::vector<std::function<void(aoihttp)>> callbacks,
                    motion *loop = uv_default_loop()) {
    lu32 len = builders.size();

    if (urls.size() != len) {
      throw std::runtime_error("Urls len and builders len should be equal.");
    }
    for (lu32 k = 0; k < len; k++) {
      async_perform_with_motion_loop(urls[k], builders[k], callbacks[k], loop);
    }
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
      bool requestSend = false;
      if (data->builder.METHOD == AOINET::_POST ||
          data->builder.METHOD == AOINET::_PUT ||
          data->builder.METHOD == AOINET::_PATCH) {
        request.setContentLength(data->builder.body.length());
        if (!data->builder.body.empty()) {
          std::ostream &os = session->sendRequest(request);
          os << data->builder.body;
          requestSend = true;
        }
      }
      if (!requestSend) {

        session->sendRequest(request);
      }
      Poco::Net::HTTPResponse response;
      std::istream &rs = session->receiveResponse(response);
      str responseText;
      Poco::StreamCopier::copyToString(rs, responseText);
      data->response = {response, responseText};

    } catch (const Poco::Exception &e) {
      std::cerr << "Exception: " << e.displayText() << "\n";
      Poco::Net::HTTPResponse resp;
      resp.setStatus("0");
      data->response = {resp, {}};
    }
  }

  /// @brief Used in the async_perform_all method, is the same method.
  /// as async_perform but with am extra argument to provide the motion loop.
  /// @param url the desired url
  /// @param builder the HTTP/Client configuration structure
  /// @param callback a callback to be called after the request is done.
  static void async_perform_with_motion_loop(
      str url, aoibuilder builder = {AOINET::_GET, DEFAULT_HEADERS, "", true},
      std::function<void(aoihttp)> callback = [](aoihttp h) { (void)h; },
      motion *loop = uv_default_loop()) {
    aoidata *data = new aoidata{url, builder, {}, {}, callback};
    data->worker.data = data;
    uv_queue_work(loop, &data->worker, aoi::async_perform_engine,
                  aoicallback::callback_perform_async);
  }
};
