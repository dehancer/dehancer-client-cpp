//
// Created by denn nevera on 2019-10-18.
//

#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

#include "dehancer/Common.h"
#include "dehancer/client/ProfileInfo.h"

namespace dehancer::camera {

    struct Format {
        std::string              id = "";
        int                      revision = 1;
        std::string              vendor_id = "";
        std::vector<std::string> types = {};
        std::string              name = "";
        std::string              description = "";

        Format() = default;
        Format(const Format&) = default;
        Format& operator=(const Format&) = default;

        Format(const dehancer::json& json);
        void dump(dehancer::json& json) const;
    };

    struct Model {

        std::string              id = "";
        std::string              name = "";
        std::string              caption = "";
        std::string              description = "";
        std::vector<std::string> formats = {};
        std::string              vendor_id = "";

        Model() = default;
        Model(const Model&) = default;
        Model& operator=(const Model&) = default;

        Model(const dehancer::json& json);
        void dump(dehancer::json& json) const;
    };

    struct Vendor {
        std::string id = "";
        std::string name = "";
        std::string caption = "";
        std::string description = "";

        Vendor() = default;
        Vendor(const Vendor&) = default;
        Vendor& operator=(const Vendor&) = default;

        Vendor(const dehancer::json &json);
        void dump(dehancer::json& json) const;
    };

    struct References {
        std::vector<Vendor> vendors;
        std::vector<Model>  models;
        std::vector<Format> formats;

        References() = default;
        References(const References&) = default;
        References&operator=(const References& ) = default;

        References(const dehancer::json &json);
        void dump(dehancer::json& json) const;
    };

    struct ProfileInfo: public network::ProfileInfoProtocol {

        std::string                 vendor_id;
        std::string                 model_id;
        std::string                 format_id;

        static int get_hash(const std::string& id) {
          return static_cast<int>(std::hash<std::string>()(id));
        }

        int get_hash() const {
          return ProfileInfo::get_hash(id);
        }

        void dump(dehancer::json& json) const;
        ProfileInfo(const dehancer::json& data);

        ProfileInfo() = default;
        ProfileInfo(const ProfileInfo&) = default;
        ProfileInfo& operator=(const ProfileInfo& other)  = default;
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
