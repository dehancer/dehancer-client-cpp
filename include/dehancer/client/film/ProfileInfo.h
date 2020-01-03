//
// Created by denn nevera on 2019-07-25.
//

#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

#include "dehancer/Common.h"
#include "dehancer/client/UserDefaults.h"
#include "dehancer/client/ProfileInfo.h"
#include "dehancer/client/film/ProfileInfo.h"

namespace dehancer::film {

    struct ProfileExports {
        uint32_t count = 0;
        uint32_t files = 0;

        ProfileExports() = default;
    };

    struct ProfileInfo: public network::ProfileInfoProtocol {

        struct Expand {
            int     mode;
            float   impact;
        };

        uint32_t    download_count = 0;
        time_t      datetime = 0;

        uint        ISO_index = 0;
        Expand      expand = {.mode=-1, .impact=0};

        ProfileExports exports = {.count=0, .files=0};

        std::string dump() const ;
        void dump(dehancer::json& json) const;
        ProfileInfo(const dehancer::json& data);

        ProfileInfo() = default;
        ProfileInfo(const ProfileInfo&);
        ProfileInfo(ProfileInfo&&) noexcept (false);
        ProfileInfo& operator=(const ProfileInfo& other);

        static int get_hash(const std::string& id) {
          return static_cast<int>(std::hash<std::string>()(id));
        }

        int get_hash() const {
          return ProfileInfo::get_hash(id);
        }

    private:
        void parse(const dehancer::json& data);
    };

    static inline bool operator==(const ProfileInfo& lhs, const ProfileInfo& rhs)
    {
      return (lhs.revision == rhs.revision) && (lhs.id == rhs.id);
    }

    static inline bool operator>(const ProfileInfo& lhs, const ProfileInfo& rhs)
    {
      if (lhs.id == rhs.id) return (lhs.revision > rhs.revision);
      return (lhs.id > rhs.id);
    }

    static inline bool operator<(const ProfileInfo& lhs, const ProfileInfo& rhs)
    {
      if (lhs.id == rhs.id) return (lhs.revision < rhs.revision);
      return (lhs.id < rhs.id);
    }

    static inline const bool contains(const std::vector<ProfileInfo>& container, const ProfileInfo& element)
    {
      return (std::find(container.begin(), container.end(), element) != container.end());
    }

    static inline const bool contains(const std::vector<ProfileInfo>& container, const std::string& id)
    {
      return (std::find_if(container.begin(), container.end(), [id](ProfileInfo const& item)
      {
          return item.id == id;
      }) != container.end());
    }
}
