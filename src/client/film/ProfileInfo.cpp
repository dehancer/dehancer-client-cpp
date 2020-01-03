//
// Created by denn nevera on 2019-07-25.
//

#include "dehancer/client/film/ProfileInfo.h"

namespace dehancer::film {

    ProfileInfo::ProfileInfo(const dehancer::json &profile):
            network::ProfileInfoProtocol()
    {
      parse(profile);
    }

    ProfileInfo::ProfileInfo(const dehancer::film::ProfileInfo& info):
            network::ProfileInfoProtocol()
    {
      dehancer::json json;
      info.dump(json);
      parse(json);
    }

    ProfileInfo::ProfileInfo(dehancer::film::ProfileInfo && info) noexcept(false):
            network::ProfileInfoProtocol()
    {
      dehancer::json json;
      info.dump(json);
      parse(json);
    }

    ProfileInfo& ProfileInfo::operator=(const dehancer::film::ProfileInfo &other) {
      dehancer::json json;
      other.dump(json);
      parse(json);
      return  *this;
    }

    void ProfileInfo::parse(const dehancer::json &profile) {

      id = profile.at("id");
      authors = profile.at("author");
      caption = profile.at("caption");
      maintainer = profile.at("maintainer");
      revision = profile.at("revision");
      url = profile.at("url");
      file_size = profile.at("file_size");

      if (profile.count("download_count")>0) download_count = profile.at("download_count");

      if (profile.count("exports")>0){
        exports.count = profile.at("exports")["count"];
        exports.files = profile.at("exports")["files"];
      }

      if (profile.count("updated_at")>0) updated_at = profile.at("updated_at");

      datetime = profile.at("datetime");

      if (profile.count("description")>0) description = profile.at("description");

      if (profile.count("tags")>0) tags = profile.at("tags");

      if (profile.count("local_path")>0)
        local_path = profile.at("local_path");
      else
        local_path = "";

      if (profile.count("is_published")>0) is_published = profile["is_published"];
      if (profile.count("is_photo_enabled")>0) is_photo_enabled = profile.at("is_photo_enabled");
      if (profile.count("is_video_enabled")>0) is_video_enabled = profile.at("is_video_enabled");
      if (profile.count("license_matrix")) license_matrix = profile["license_matrix"].get<std::vector<License::Type >>();

      if (profile.count("ISO_index")>0) ISO_index = profile.at("ISO_index");

      if (profile.count("expand_mode")>0) expand.mode = profile.at("expand_mode");

      if (profile.count("expand_impact")>0) expand.impact = profile.at("expand_impact");
    }

    void ProfileInfo::dump(dehancer::json &json) const {
      json.clear();
      json["id"] = id;
      json["author"] = authors;
      json["caption"] = caption;
      json["url"] = url;
      json["local_path"] = local_path;
      json["maintainer"] = maintainer;
      json["revision"] = revision;
      json["file_size"] = file_size;
      json["download_count"] = download_count;
      json["exports"] = {{"count",exports.count},{"files",exports.files}};
      json["updated_at"] = updated_at;
      json["datetime"] = datetime;
      json["description"] = description;
      json["tags"] = tags;
      json["ISO_index"] = ISO_index;
      json["expand_mode"] = expand.mode;
      json["expand_impact"] = expand.impact;
      json["is_published"] = is_published;
      json["is_video_enabled"] = is_video_enabled;
      json["is_photo_enabled"] = is_photo_enabled;
      json["license_matrix"] = license_matrix;

    }

    std::string ProfileInfo::dump() const {
      std::string _dump;
      _dump.append("id               : " + id + "\n");
      _dump.append("revision         : " + std::to_string(revision) + "\n");
      _dump.append("url              : " + url + "\n");
      _dump.append("caption          : " + caption + "\n");
      _dump.append("local_path       : " + local_path + "\n");
      _dump.append("ISO_index        : " + std::to_string(ISO_index) + "\n");
      _dump.append("expand_mode      : " + std::to_string(expand.mode) + "\n");
      _dump.append("expand_impact    : " + std::to_string(expand.impact) + "\n");
      _dump.append("file_size        : " + std::to_string(file_size) + "\n");
      _dump.append("is_video_enabled : " + std::to_string(is_video_enabled) + "\n");
      _dump.append("is_photo_enabled : " + std::to_string(is_photo_enabled) + "\n");
      _dump.append("developing count : " + std::to_string(exports.count) + "\n");
      _dump.append("developing files : " + std::to_string(exports.files) + "\n");

      return _dump;
    }

}