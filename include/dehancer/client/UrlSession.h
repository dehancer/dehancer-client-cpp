//
// Created by denn nevera on 2019-12-30.
//

#pragma once

#include "dehancer/Common.h"
#include "dehancer/Semaphore.h"
#include "dehancer/client/HttpMessage.h"

#include <rxcpp/rx.hpp>

namespace dehancer::network {

    namespace client {

        namespace rx=rxcpp;

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

            using PathHandler = std::function<std::string_view(const UrlSession &session)>;

        public:

            /**
             * Create UrlSession object
             * @param url - url string
             * @param timeout - default request time out
             */
            explicit UrlSession(const std::string &url, std::time_t timeout = 60);

            UrlSession() = delete;
            UrlSession(UrlSession&&) = delete;
            UrlSession(const UrlSession&) = delete;

            /**
             * Get current session url
             * @return url string
             */
            const std::string &get_url() const;

            /**
             * Make async http request and get observable response object
             * @param request - http request body
             * @return observable response object
             */
            rx::observable<std::shared_ptr<HttpResponse>> request(const HttpRequest &request) const;

            /**
             *
             * Download data from url
             *
             * @param handler - handler download callback helper
             * @param request -  http request body
             * @return observable response object
             */
            rx::observable<std::shared_ptr<HttpResponse>> download(
                    const PathHandler &handler,
                    const HttpRequest &request = {}
            ) const;

        private:
            std::shared_ptr<detail::Session> session_;
        };
    }
}

