//
// Created by denn nevera on 2019-07-25.
//

#include "UserMacOs.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <mutex>

#ifdef __APPLE__

#import <Foundation/Foundation.h>

namespace dehancer::platform {

    static std::mutex  mutex;

    static dehancer::expected<dehancer::json,Error> read_pref(const std::string& path, const std::string& dir);

    UserMacOs::UserMacOs(const std::string& container_id):
            User(container_id),
            //container_id_(container_id),
            user_id_([NSUserName() UTF8String]),
            user_name_([NSFullUserName() UTF8String]),
            storage_dir_([NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, true)[0] UTF8String]),
            key_chain_(container_id)
    {
      storage_dir_.append("/");
      storage_dir_.append(get_container_id());
      preference_file_path_ = storage_dir_;
      preference_file_path_.append("/");
      preference_file_path_.append("preferences.json");
    }

    const Error UserMacOs::set_secure(const dehancer::json &value, const std::string &key) {
        return key_chain_.set_object(value,key);
    }

  dehancer::expected<dehancer::json,Error> UserMacOs::get_secure(const std::string &key) const {
      return key_chain_.get_object(key);
    }

    const std::string & UserMacOs::get_storage_dir() const {
        return storage_dir_;
    }

    const std::string & UserMacOs::get_preference_path() const {
        return preference_file_path_;
    }

    const std::string & UserMacOs::get_id() const {
        return user_id_;
    }

    const std::string& UserMacOs::get_name() const {
        return user_name_;
    }

    dehancer::expected<dehancer::json,Error> UserMacOs::get_defaults(const std::string &key) const {
        std::lock_guard lock(mutex);

        try {

            auto data = read_pref(preference_file_path_, storage_dir_);

            if (!data) {
                return data;
            }

            if (data->count(key)>0) {
                return data->at(key);
            }

            return dehancer::json();

        }
        catch (std::exception &err) {
            return dehancer::make_unexpected(Error(CommonError::PARSE_ERROR, err.what()));
        }

    }

    const Error UserMacOs::set_defaults(const dehancer::json &value, const std::string &key) const {

        std::lock_guard lock(mutex);

        try{
            auto data = read_pref(preference_file_path_, storage_dir_);

            if (!data)
                return data.error();

            data.value()[key] = value;

            std::vector<char> buffer;

            dehancer::json::to_msgpack(data.value(), buffer);

            std::ofstream outFile;
            outFile.open(preference_file_path_.c_str(),  std::fstream::out);

            if (!outFile.is_open()) {
                Error(dehancer::CommonError::NOT_FOUND,
                      error_string("file %s could not be opened or found", preference_file_path_.c_str()));
            }

            std::copy(buffer.begin(), buffer.end(), std::ostreambuf_iterator<char>(outFile));

            return Error(CommonError::OK);

        }
        catch (std::exception &err) {
            return Error(CommonError::PARSE_ERROR, err.what());
        }
    }

    dehancer::expected<dehancer::json,Error> read_pref(const std::string& path, const std::string& dir){

        if (access(dir.c_str(), R_OK) != 0 ) {
            if (mkdir(dir.c_str(),S_IRUSR|S_IWUSR|S_IXUSR) !=0 ){
                return dehancer::make_unexpected(Error(
                        dehancer::CommonError::PERMISSIONS_ERROR,
                        error_string("dir %s could not be created", dir.c_str())));
            }
        }

        std::ifstream inFile;
        inFile.open(path.c_str(),  std::fstream::in);

        if (!inFile.is_open())
            return dehancer::json();

        std::vector<char> buffer;

        inFile.seekg(0, std::ios::end);
        buffer.reserve(inFile.tellg());
        inFile.seekg(0, std::ios::beg);

        buffer.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

        if (buffer.empty())
            return dehancer::json();

        return dehancer::json::from_msgpack(buffer);

    }
}

#endif