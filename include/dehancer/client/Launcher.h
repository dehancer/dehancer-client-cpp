//
// Created by denn nevera on 2019-08-19.
//

#pragma once

#include "dehancer/Common.h"
#include <memory>

namespace dehancer {

    namespace platform {

        /**
         *
         */
        class LauncherProtocol {
        public:

            virtual Error set_url(const std::string &path) = 0 ;
            virtual void set_arguments(const std::vector<std::string>& args) = 0;
            virtual Error open() = 0 ;

            virtual ~LauncherProtocol() {};

        };


        class Launcher: public LauncherProtocol{

        public:
            Launcher();

            virtual Error set_url(const std::string &path) override ;
            virtual void set_arguments(const std::vector<std::string>& args) override;
            virtual Error open() override ;

            virtual ~Launcher() {};

        private:
            std::unique_ptr<LauncherProtocol> impl_;
        };
    }
}

