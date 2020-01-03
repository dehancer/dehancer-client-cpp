//
// Created by denn nevera on 2020-01-03.
//

#include "dehancer/client/UserDefaults.h"
#include "ed25519.hpp"
#include <memory>

#ifdef __APPLE__

#include "platform/macos/UserMacOs.h"
#import <Foundation/Foundation.h>

#endif

namespace dehancer::platform {

    std::shared_ptr<User> User::Defaults(const std::string &container_id) {

#ifdef __APPLE__
      auto user = new UserMacOs(container_id);
#else
#error "User defaults for this platofrom is not implemented yet..."
#endif

      auto some_data = user->get_defaults("__DEHANCERD_CUID_KEY__");

      if (!some_data && some_data.error().value() != static_cast<int>(CommonError::NOT_FOUND)){
        return nullptr;
      }

      if (!some_data || some_data->empty()) {

        some_data = json();

        auto p = ed25519::keys::Pair::Random();
        user->cuid_.first = p->get_public_key().encode();
        user->cuid_.second = p->get_private_key().encode();
        some_data.value()["__PK__"] = user->cuid_.first;
        some_data.value()["__PVK__"] = user->cuid_.second;

        user->set_defaults(*some_data, "__DEHANCERD_CUID_KEY__");

      } else {

        user->cuid_.first = some_data->at("__PK__");
        user->cuid_.second = some_data->at("__PVK__");

      }
      
      return std::shared_ptr<User>(user);
    }

}