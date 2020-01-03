//
// Created by denn nevera on 2020-01-03.
//

#pragma once

#include <string>
#include <map>
#include <time.h>
#include <atomic>
#include <functional>
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#endif

#include "dehancer/client/UserDefaults.h"
#include "dehancer/client/ProfileInfo.h"

#include "dehancer/client/film/ProfileInfo.h"
#include "dehancer/client/camera/ProfileInfo.h"
#include "promisecpp/promise.hpp"

namespace dehancer::network::client {

    using DeferHandler  = std::function<void(promise::Defer d)>;

    namespace detail {
        struct RpcClient;
    }

    /**
     * Dehacnerd client API
     */
    class Rpc {
    public:

        using Result = dehancer::expected<dehancer::json,dehancer::Error>;

    public:

        /***
         * Create client connection
         * @param url - dehancerd server url
         * @param container_id - user container unique id (com.dehancer.film.xx, for example)
         * @param api_access_token_private - dehancerd access token
         * @param api_acces_name - registered client access name
         * @param timeout - network operations timeout in seconds
         * @param reconnections - reconnections when error occur
         * @param reconnection_delay = timeout between reconnections
         */
        Rpc(const std::string& url,
            const std::string& user_container_id,
            const std::string& api_access_token_private,
            const std::string& api_acces_name,
            time_t timeout = 60,
            int    reconnections = 1,
            time_t reconnection_delay = 1000
        );

        Rpc() = delete;
        Rpc(Rpc&&) = delete;
        Rpc(const Rpc&) = delete;

        /**
         * Get current access token
         * @return - access tokein base58 string
         */
        const std::string& get_access_token() const;

        /**
         * Login on server
         * @return A+ promise with async result, contains pointer to self Client* object or Error
         */
        promise::Defer login();

        /**
         * Get client authenticate token
         * @return deferred token object
         */
        promise::Defer get_auth_token();

        /***
         * Send user info (optional)
         * @return A+ promise with async result, contains pointer to self Client* object or Error
         */
        promise::Defer set_user_info();

        /***
         * Get profile list
         * @return A+ promise contains std::vector<dehancer::camera::Vendor,Model,Format> - list
         */
        promise::Defer get_camera_references();

        /***
         * Get profile list
         * @return A+ promise contains std::vector<dehancer::camera::ProfileInfo> - list
         */
        promise::Defer get_camera_profile_list();

        /***
         * Get profile list
         * @return A+ promise contains std::vector<dehancer::film::ProfileInfo> - profile list
         */
        promise::Defer get_film_profile_list();

        /**
         * Activate license or check local license file
         * @return  A+ promise contains std::vector<dehancer::License> - profile list
         */
        promise::Defer activate_license(const std::string& base64key);

        /**
        * Deactivate license for the activation key
        * @return  A+ promise contains std::vector<dehancer::License> - profile list
        */
        promise::Defer deactivate_license(const std::string& base64key);

        /**
         * Get mutable user prefs container
         * @return user object
         */
        platform::User& get_user();

        /**
         * Get client url
         * @return url string
         */
        const std::string& get_url() const;

    private:
        std::shared_ptr<detail::RpcClient> impl_;
//        std::string url_;
//        platform::User user_;
//        std::pair<std::string,std::string> cuid_;
//        std::pair<std::string,std::string> api_access_pair_;
//        std::string api_acces_name_;
//        time_t timeout_;
//        std::atomic_uint32_t request_id_;
//        std::string access_token_;
//
//        promise::Defer execute(const DeferHandler& handler);
//
//        Result get_cuid_state_request(const std::string& token);
//        Result get_auth_token_request();
//        Result set_user_info_request();
//        Client::Result get_request(const std::string &method, const dehancer::json &ext_params);
//        Result license_request(const std::string &method, const std::string &base64key);
    };

}

