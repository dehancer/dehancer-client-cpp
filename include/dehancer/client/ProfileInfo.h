//
// Created by denn nevera on 2019-10-20.
//

#pragma once

#include <string>
#include <dehancer/License.h>

namespace dehancer::network {

    struct  ProfileInfoProtocol {
        std::string id = "";
        std::string url = "";
        size_t      file_size  = 0;
        int         revision   = 0;
        std::time_t updated_at = 0;
        std::string local_path = "";
        std::string caption    = "";
        std::string description = "";
        std::string authors = "";
        std::string maintainer = "";
        std::string tags = "";
        bool        is_published = false;
        bool        is_photo_enabled = true;
        bool        is_video_enabled = true;
        std::vector<License::Type>  license_matrix = {};

        ProfileInfoProtocol get_copy() const {
          return (ProfileInfoProtocol) {
                  id,
                  url,
                  file_size,
                  revision,
                  updated_at,
                  local_path,
                  caption,
                  description,
                  authors,
                  maintainer,
                  tags,
                  is_published,
                  is_photo_enabled,
                  is_video_enabled,
                  license_matrix
          };
        }

        void set_copy(const ProfileInfoProtocol& p) {
          id = p.id;
          url = p.url;
          file_size = p.file_size;
          revision = p.revision;
          updated_at = p.updated_at;
          local_path = p.local_path;
          caption = p.caption;
          description = p.description;
          authors = p.authors;
          maintainer = p.maintainer;
          tags = p.tags;
          is_published = p.is_published;
          is_photo_enabled = p.is_photo_enabled;
          is_video_enabled = p.is_video_enabled;
          license_matrix = p.license_matrix;
        }
    };
}