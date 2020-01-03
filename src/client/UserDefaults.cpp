//
// Created by denn nevera on 2020-01-03.
//

#include "dehancer/client/UserDefaults.h"
#include <memory>

#ifdef __APPLE__

#include "platform/macos/UserMacOs.h"
#import <Foundation/Foundation.h>

#endif

namespace dehancer::platform {

    std::shared_ptr<User> User::Defaults(const std::string &container_id) {
#ifdef __APPLE__
      return std::shared_ptr<User>(new UserMacOs(container_id));
#else
#error "User defaults for this platofrom is not implemented yet..."
#endif
    }

}