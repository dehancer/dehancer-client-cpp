//
// Created by denn nevera on 2019-10-18.
//

#include "dehancer/client/camera/ProfileInfo.h"


namespace dehancer::camera {

    ///
    /// MARK - Format
    ///

    Format::Format(const dehancer::json &json) {
        if (json.count("id")) id = json["id"];
        if (json.count("revision")) revision = json["revision"];
        if (json.count("vendor_id")) vendor_id = json["vendor_id"];
        if (json.count("types")) types = json["types"].get<std::vector<std::string>>();
        if (json.count("name")) name = json["name"];
        if (json.count("description")) description = json["description"];
    }

    void Format::dump(dehancer::json &json) const {
        json.clear();
        json["id"] = id;
        json["revision"] = revision;
        json["vendor_id"] = vendor_id;
        json["types"] = types;
        json["name"] = name;
        json["description"] = description;
    }


    ///
    /// MARK - Model
    ///

    Model::Model(const dehancer::json &json) {
        if (json.count("id")) id = json["id"] ;
        if (json.count("name")) name = json["name"];
        if (json.count("caption")) caption = json["caption"] ;
        if (json.count("description")) description = json["description"];
        if (json.count("formats")) formats = json["formats"].get<std::vector<std::string>>();
        if (json.count("vendor_id")) vendor_id = json["vendor_id"];
    }

    void Model::dump(dehancer::json &json) const {
        json.clear();
        json["id"] = id;
        json["name"] = name;
        json["caption"] = caption;
        json["description"] = description;
        json["formats"] = formats;
        json["vendor_id"] = vendor_id;
    }

    ///
    /// MARK - Vendor
    ///

    Vendor::Vendor(const dehancer::json &json) {
        if (json.count("id")) id = json["id"];
        if (json.count("name")) name = json["name"];
        if (json.count("caption")) caption = json["caption"];
        if (json.count("description")) description = json["description"];
    }

    void Vendor::dump(dehancer::json &json) const {
        json.clear();

        json["id"] = id;
        json["name"] = name;
        json["caption"]=caption;
        json["description"]=description;
    }


    ///
    /// References
    ///

    References::References(const dehancer::json &json) {
        if (json.count("camera_formats")) {
            for (auto f: json["camera_formats"]) {
                formats.push_back(Format(f));
            }
        }

        if (json.count("camera_models")) {
            for (auto m: json["camera_models"]) {
                models.push_back(Model(m));
            }
        }

        if (json.count("camera_vendors")) {
            for (auto v: json["camera_vendors"]) {
                vendors.push_back(Vendor(v));
            }
        }
    }

    void References::dump(dehancer::json &json) const {
        json.clear();

        json["camera_vendors"] = std::vector<dehancer::json>();

        for(auto v: vendors) {
            dehancer::json vndr;
            v.dump(vndr);
            json["camera_vendors"].push_back(vndr);
        }

        json["camera_models"] = std::vector<dehancer::json>();

        for(auto v: models) {
            dehancer::json mdls;
            v.dump(mdls);
            json["camera_models"].push_back(mdls);
        }

        json["camera_formats"] = std::vector<dehancer::json>();

        for(auto v: formats) {
            dehancer::json frmt;
            v.dump(frmt);
            json["camera_formats"].push_back(frmt);
        }
    }

    ///
    /// MARK - ProfileInfo
    ///

    ProfileInfo::ProfileInfo(const dehancer::json &json):
            network::ProfileInfoProtocol()
    {
        if (json.count("id")) id = json["id"];
        if (json.count("caption")) caption = json["caption"];
        if (json.count("description")) description = json["description"];
        if (json.count("url")) url = json["url"] ;
        if (json.count("revision")) revision = json["revision"];
        if (json.count("file_size")) file_size = json["file_size"];
        if (json.count("updated_at")) updated_at = json["updated_at"];
        if (json.count("local_path")) local_path = json["local_path"];

        if (json.count("is_photo_enabled")) is_photo_enabled = json["is_photo_enabled"];
        if (json.count("is_published")) is_published = json["is_published"];
        if (json.count("is_video_enabled")) is_video_enabled = json["is_video_enabled"];
        if (json.count("license_matrix")) license_matrix = json["license_matrix"].get<std::vector<License::Type >>();

        if (json.count("vendor_id")) vendor_id = json["vendor_id"];
        if (json.count("model_id")) model_id = json["model_id"];
        if (json.count("format_id")) format_id = json["format_id"];

        if (json.count("author")) authors = json["author"];
        if (json.count("maintainer")) maintainer = json["maintainer"];
        if (json.count("tags")) tags = json["tags"];
    }

    void ProfileInfo::dump(dehancer::json &json) const {
        json.clear();

        json["id"] =  id;
        json["caption"] =  caption;
        json["description"] =  description;
        json["url"] = url;
        json["revision"] = revision;
        json["updated_at"] = updated_at;
        json["file_size"] = file_size;
        json["local_path"] = local_path;

        json["is_photo_enabled"] = is_photo_enabled;
        json["is_published"] = is_published;
        json["is_video_enabled"] = is_video_enabled;
        json["license_matrix"] = license_matrix;

        json["vendor_id"] = vendor_id;
        json["model_id"] = model_id;
        json["format_id"] = format_id;

        json["author"] = authors;
        json["maintainer"] = maintainer;
        json["tags"] = tags;

    }

}