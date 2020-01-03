//
// Created by denn nevera on 2019-08-19.
//

#pragma once

#include "dehancer/client/Launcher.h"
#include "dehancer/Common.h"
#include "dehancer/Utils.h"
#include <string>

#if __APPLE__
#import "Foundation/Foundation.h"
#endif

namespace dehancer{
    namespace platform {

        class LauncherMacOs: public LauncherProtocol{

        public:
            explicit LauncherMacOs();
            LauncherMacOs(LauncherMacOs&&) = default;
            LauncherMacOs(const LauncherMacOs&) = default;

            virtual Error set_url(const std::string &path) override ;
            virtual void set_arguments(const std::vector<std::string>& args) override;
            virtual Error open() override ;

            virtual ~LauncherMacOs() {};

        private:
#if __APPLE__
            NSTask *task_;
#endif
        };
    }
}