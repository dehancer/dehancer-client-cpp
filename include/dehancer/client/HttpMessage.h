//
// Created by denn nevera on 2020-01-03.
//

//
// Created by denn nevera on 2019-12-30.
//

#pragma once

#include "dehancer/Common.h"
#include "dehancer/Semaphore.h"

#include <curl/curl.h>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <fstream>

namespace dehancer::network {

    /**
     * Http message common
     * */
    struct HttpMessage {
        /**
         * message headers
         * */
        std::map<std::string, std::string> headers;
    };

    /**
     * Http request
     *
     * client::HttpRequest request = {
     *     {
     *             .headers = {},
     *     },
     *     .method = client::HttpRequest::Method::post
     *     .body = "request body, json-rpc for example"
     * };
     *
     * */
    struct HttpRequest : public HttpMessage {
        /**
         * available http request methods
         */
        enum Method : int {
            get = 1000,
            post
        };
        /**
         * The method of the request
         */
        Method method = Method::get;
        /**
         * request body
         */
        std::string body = "";
    };

    /**
     * Http response protocol
     * */
    struct HttpResponse : public HttpMessage {
        /**
         * Http response state
         */
        enum State : int {
            undefined = 0,
            in_progress,
            completed
        };

        /**
         * Current state of the container
         */
        State state = State::undefined;

        /**
         * Received http status
         */
        long http_status = 0;

        /**
         * Received content length if server set Content-Length header
         */
        std::size_t content_length = 0;

        /**
         * Already received content length
         */
        std::size_t received_length = 0;

        /**
         * Progression stage in percent: [0,100]
         */
        short progress = 0; // 0-100
        /**
         * response message body
         */

        /**
         * Append partial data must be implemented in certain class
         * @param chunk - part of recieved data
         */
        virtual void append(const std::vector<std::uint8_t> &chunk) = 0;

        /**
         * Write completed data to buffer
         * @param buffer - emplace buffer
         */
        virtual void write(std::vector<std::uint8_t> &buffer) = 0;

        /**
         * Write data to string
         * @param buffer
         */
        virtual void write(std::string &buffer);

        virtual ~HttpResponse();
    };

    /**
     * Http response message keeping in memory
     */
    struct HttpResponseMessage : HttpResponse {

        void append(const std::vector<std::uint8_t> &chunk) override;

        void write(std::vector<std::uint8_t> &buffer) override;

    private:
        std::vector<std::uint8_t> body_;
    };


    struct HttpResponseFile : HttpResponse {
        HttpResponseFile(const std::string_view &file);

        void append(const std::vector<std::uint8_t> &chunk) override;

        void write(std::vector<std::uint8_t> &buffer) override;

    private:
        std::string path_;
        std::unique_ptr<std::ofstream> outFile_;
    };

    /**
            * Common session exceptions
            */
    struct exception : public std::runtime_error {

        using std::runtime_error::runtime_error;

        explicit exception(CURLcode code, std::shared_ptr<HttpResponse> response = nullptr);

        explicit exception(const std::string &error, std::shared_ptr<HttpResponse> response);

        CURLcode get_code() const { return code_; }

        long get_http_status() const { return status_; }

        std::shared_ptr<HttpResponse> get_response() const { return response_; }

    private:
        CURLcode code_;
        long status_;
        std::shared_ptr<HttpResponse> response_;
    };

}

