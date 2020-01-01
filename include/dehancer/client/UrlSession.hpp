//
// Created by denn nevera on 2019-12-30.
//

#pragma once

#include "dehancer/Common.h"
#include "dehancer/Semaphore.h"

#include <rxcpp/rx.hpp>
#include <curl/curl.h>
#include <string>
#include <memory>
#include <map>

namespace dehancer::network::client {

    namespace rx=rxcpp;

    /**
     * Http message common
     * */
    struct HttpMessage {
        /**
         * message headers
         * */
        std::map<std::string, std::string> headers;
        /**
         * message body
         */
        std::string                        body;
    };

    /**
     * Http request
     *
     * client::HttpRequest request = {
     *     {
     *             .headers = {},
     *             .body = "request body, json-rpc for example"
     *     },
     *     .method = client::HttpRequest::Method::post
     * };
     *
     * */
    struct HttpRequest: public HttpMessage {
        /**
         * available http request methods
         */
        enum Method:int {
            get = 1000,
            post
        };
        /**
         * The method of the request
         */
        Method                             method;
    };

    /**
     * Http response container
     * */
    struct HttpResponse:public HttpMessage {
        /**
         * Http response state
         */
        enum State:int {
            undefined = 0,
            in_progress,
            completed
        };

        /**
         * Current state of the container
         */
        State                             state = State::undefined;

        /**
         * Received http status
         */
        long                              http_status = 0;

        /**
         * Received content length if server set Content-Length header
         */
        std::size_t                       content_length = -1;

        /**
         * Progression stage in percent: [0,100]
         */
        short                             progress = 0; // 0-100
    };

    namespace detail {
        class Session;
    }

    /**
     *
     * Url session class
     *
     * */
    class UrlSession {

    public:

        /**
         * Common session exceptions
         */
        struct exception: public std::runtime_error {

            using std::runtime_error::runtime_error;

            explicit exception(CURLcode code, std::shared_ptr<HttpResponse> response = nullptr);
            explicit exception(const std::string& error, std::shared_ptr<HttpResponse> response);
            CURLcode get_code() const { return code_; }
            std::shared_ptr<HttpResponse> get_response() const { return response_;}

        private:
            CURLcode code_;
            std::shared_ptr<HttpResponse> response_;
        };

    public:

        /**
         * Create UrlSession object
         * @param url - url string
         * @param timeout - default request time out
         */
        UrlSession(const std::string& url, std::time_t timeout = 60);

        /**
         * Get current session url
         * @return url string
         */
        const std::string& get_url() const;

        /**
         * Make async http request and get observable response object
         * @param request - http request body
         * @return observable response object
         */
        rx::observable<std::shared_ptr<HttpResponse>> request(const HttpRequest& request) const ;

    private:
        std::shared_ptr<detail::Session> session_;
    };
}

