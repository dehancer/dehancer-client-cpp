//
// Created by denn nevera on 2019-07-25.
//

#pragma once

#include <string>
#import "KeyChain.h"

#import "dehancer/client/UserDefaults.h"

namespace dehancer::platform {

    class UserMacOs: public dehancer::platform::User {

    public:

        explicit UserMacOs(const std::string& container_id);

        UserMacOs(UserMacOs&&) = default;
        UserMacOs(const UserMacOs&) = default;

        const std::string& get_container_id() const override { return container_id_;};
        const std::string& get_id() const override ;
        const std::string& get_name() const override ;
        const std::string& get_storage_dir() const override ;
        const std::string& get_preference_path() const override ;

        dehancer::expected<dehancer::json,Error> get_defaults(const std::string& key) const override;
        const Error set_defaults(const dehancer::json& value, const std::string& key) const override;

        dehancer::expected<dehancer::json,Error> get_secure(const std::string& key) const override;
        const Error set_secure(const dehancer::json& value, const std::string& key) override;

    private:
        std::string container_id_;
        std::string user_id_;
        std::string user_name_;
        std::string storage_dir_;
        std::string preference_file_path_;
        KeyChain    key_chain_;
    };
}
