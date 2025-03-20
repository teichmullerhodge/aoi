#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <uv.h>
#include <vector>
#include <memory>
#include <iostream>
#include <optional>
#include <string>
#include <array>
#include <assert.h>
#include "../declarations/declarations.hpp"
#include "aoimotion.hpp"


class aoi {

    private:

        static void set_headers(Poco::Net::HTTPRequest& req, std::vector<aoiheaders> headers){
            
            for(const auto& h : headers){
                req.set(h.first, h.second);
            }
        }

    public:

        static bool status_ok(u16 status) {
            return status >= 200 && status <= 299;
        }

        static aoihttp get(str url, aoibuilder builder = { AOINET::_GET, DEFAULT_HEADERS, "", true}) {
            try {

                Poco::URI uri(url);
                std::unique_ptr<Poco::Net::HTTPClientSession> session;
                if(builder.useSSL){
                    session = std::make_unique<Poco::Net::HTTPSClientSession>(uri.getHost(), uri.getPort());
                } else {
                    session = std::make_unique<Poco::Net::HTTPClientSession>(uri.getHost(), uri.getPort());
                }

                Poco::Net::HTTPRequest request(AOINET::_GET, uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);
                request.set("Host", uri.getHost());
                set_headers(request, builder.headers);                
                session->sendRequest(request);
                Poco::Net::HTTPResponse response;
                std::istream& rs = session->receiveResponse(response);
                str responseText;
                Poco::StreamCopier::copyToString(rs, responseText);
                aoihttp r = {response, responseText};
                return r;

            } catch (const Poco::Exception& e) {
                std::cerr << "Exception: " << e.displayText() << '\n';
                return aoihttp{};
            }
        }

        static aoihttp perform(str url, aoibuilder builder = { AOINET::_GET, DEFAULT_HEADERS, "", true}) {
            try {
            Poco::URI uri(url);
            std::unique_ptr<Poco::Net::HTTPClientSession> session;
            if(builder.useSSL){
                session = std::make_unique<Poco::Net::HTTPSClientSession>(uri.getHost(), uri.getPort());
            } else {
                session = std::make_unique<Poco::Net::HTTPClientSession>(uri.getHost(), uri.getPort());
            }

            Poco::Net::HTTPRequest request(builder.METHOD, uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);
            request.set("Host", uri.getHost());
            set_headers(request, builder.headers);
            
            if (builder.METHOD == AOINET::_POST || builder.METHOD == AOINET::_PUT) {
                if (!builder.body.empty()) {
                    std::ostream& os = session->sendRequest(request);
                    os << builder.body;
                }
            }
            
            session->sendRequest(request);
            std::ostringstream oss;
            Poco::Net::HTTPResponse response;
            std::istream& rs = session->receiveResponse(response);
            str responseText;
            Poco::StreamCopier::copyToString(rs, responseText);
            aoihttp r = {response, responseText};
            return r;

        } catch (const Poco::Exception& e) {
            std::cerr << "Exception: " << e.displayText() << '\n';
            return aoihttp{};
        }
    }

    static void perform_async(engine* req){
        MotionRequest *request = (MotionRequest*)req->data;
        aoibuilder b = {
            request->METHOD,
            request->headers,
            request->body,
            request->useSSL
        };

        request->result = perform(request->url, b);
    }
    static void perform_async_callback(engine* req, i32 status){
        MotionRequest *response = (MotionRequest *)req->data;
        if (status < 0) {
            fprintf(stderr, "Error in worker thread: %s\n", uv_strerror(status));
        } else {
            printf("Returned: %d\n", response->result.get_status());
        }
        free(response);
    
    }
    static std::vector<aoihttp> perform_as_batch(std::vector<str> urls, std::vector<aoibuilder> builders){

        lu32 len = builders.size();
        if(urls.size() != builders.size()){
            throw std::runtime_error("The urls and builder should have the same size.");
        }
        motion *loop = uv_default_loop();
        std::vector<aoihttp> result;
        result.reserve(len);
        for(size_t k = 0; k < len; k++){

            MotionRequest* req = new MotionRequest();
            req->body.assign(builders[k].body);
            req->id = k;
            req->headers = builders[k].headers;
            req->METHOD = builders[k].METHOD;
            req->useSSL = builders[k].useSSL;            
            req->worker.data = (void*)req;
            req->url = urls.at(k);
            result.push_back(req->result);
            uv_queue_work(loop, &req->worker, perform_async, perform_async_callback);

        }
        
        uv_run(loop, UV_RUN_DEFAULT);
        uv_loop_close(loop);
        return result;
        
    }



};
