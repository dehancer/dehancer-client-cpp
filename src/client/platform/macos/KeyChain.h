//
// Created by denn nevera on 2019-08-09.
//

#pragma once

#include "dehancer/Common.h"
#include <string>
#include <vector>

namespace dehancer::platform {

    enum class ItemClass: int {
        GENERIC = 1,
        INTERNET
    };

    class KeyChain {

    public:
        KeyChain(const std::string& service_name="com.dehancer.app");

        Error set_object(const dehancer::json &object,
                         const std::string &for_key,
                         const std::string &label ="",
                         const std::string &comment ="");
        expected<json,Error> get_object(const std::string& for_key) const;
        Error remove_object(const std::string& for_key);

    private:
        std::string service_name_;
    };
}