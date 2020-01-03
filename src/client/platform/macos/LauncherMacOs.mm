//
// Created by denn nevera on 2019-08-19.
//

#include "LauncherMacOs.h"

#ifdef __APPLE__

namespace dehancer {
    namespace platform {

        LauncherMacOs::LauncherMacOs():task_([[NSTask alloc] init]){
        }

        Error LauncherMacOs::open() {

            NSError *error;

            [task_ launchAndReturnError:&error];

            [task_ waitUntilExit];

            if (error) {
                return Error(CommonError::NOT_SUPPORTED, [[error localizedDescription] UTF8String]);
            }

            return Error(CommonError::OK);
        }

        Error LauncherMacOs::set_url(const std::string &path) {

            auto url = [NSURL fileURLWithPath: [NSString stringWithUTF8String:path.c_str()]];

            if (access(path.c_str(), R_OK) == 0 ) {

                auto name = [[url URLByDeletingPathExtension] lastPathComponent];

                url = [url URLByAppendingPathComponent:@"Contents"];
                url = [url URLByAppendingPathComponent:@"MacOS"];
                url = [url URLByAppendingPathComponent:name];

                NSError *error;

                if (![url checkResourceIsReachableAndReturnError:&error]) {
                    if (error)
                        return Error(CommonError::NOT_FOUND, [[error localizedDescription] UTF8String]);
                }

                [task_ setExecutableURL: url];

                return Error(CommonError::OK);
            }
            return Error(CommonError::NOT_FOUND);
        }

        void LauncherMacOs::set_arguments(const std::vector<std::string>& args) {
            NSMutableArray<NSString*> *list = [NSMutableArray new];
            for (auto arg: args) {
                [list addObject:[NSString stringWithUTF8String:arg.c_str()]];
            }
            [task_ setArguments:list];
        }

    }
}
#endif