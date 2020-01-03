//
// Created by denn nevera on 2020-01-03.
//

//
// Created by denn nevera on 2019-10-17.
//

#pragma once

#include "dehancer/Common.h"
#include "dehancer/Utils.h"
#include <string>
#include <memory>

namespace dehancer::platform {

    /**
     * User defaults container
     */
    class User {

    protected:
        explicit User(const std::string& container_id);

    public:

        static std::shared_ptr<User> Defaults(const std::string& container_id);

        User() = delete;
        User(User&&) = delete;
        User(const User&) = delete;

        /**
         * Get the current user application container identifier
         * @return user container id string
         */
        virtual const std::string &get_container_id();

        /**
         * Get the user OS identifier
         * @return user OS id string
         */
        virtual const std::string &get_id() const = 0;

        /**
         * Get the user name
         * @return user name string
         */
        virtual const std::string &get_name() const = 0;

        /**
         * Get crypto pair as a global unique user id
         * @return ed25519 decoded strings pair
         */
        virtual const std::pair<std::string,std::string>& get_cuid();

        /**
         * Get the user storage directory
         * @return get user directory path string
         */
        virtual const std::string &get_storage_dir() const = 0;

        /**
         * Get the user preferences container file path
         * @return file path string
         */
        virtual const std::string &get_preference_path() const = 0;

        /**
         * Get the user default expected container or error if container can't be read.
         * Container is a persistence key:value storage.
         * @param key
         * @return value is stored by the key
         */
        virtual dehancer::expected<dehancer::json, Error> get_defaults(const std::string &key) const = 0;

        /**
         * Put a value to the user persistence storage with a certain key
         * @param value
         * @param key
         * @return error or OK
         */
        virtual const Error set_defaults(const dehancer::json &value, const std::string &key) const = 0;

        /**
         * Get a value from secure user container that keep all values in crypto container
         * @param key
         * @return decrypted value
         */
        virtual dehancer::expected<dehancer::json, Error> get_secure(const std::string &key) const = 0;

        /**
         * Put a value to the user crypto container
         * @param value
         * @param key
         * @return error or OK
         */
        virtual const Error set_secure(const dehancer::json &value, const std::string &key) = 0;

        virtual ~User() = default;

    private:
        std::string container_id_;
        std::pair<std::string,std::string> cuid_;
    };
}