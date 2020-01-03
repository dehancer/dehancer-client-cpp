//
// Created by denn nevera on 2020-01-03.
//

#include "dehancer/client/HttpMessage.h"

namespace dehancer::network {
    HttpResponse::~HttpResponse() {}

    void HttpResponse::write(std::string &buffer) {
      std::vector<std::uint8_t> data;
      this->write(data);
      buffer.assign(data.begin(), data.end());
    }

    void HttpResponseMessage::append(const std::vector<std::uint8_t> &chunk) {
      body_.insert(body_.end(), chunk.begin(), chunk.end());
    }

    void HttpResponseMessage::write(std::vector<std::uint8_t> &buffer) {
      buffer = body_;
    }

    HttpResponseFile::HttpResponseFile(const std::string_view &file):
            path_(file),outFile_(new std::ofstream) {
      outFile_->open(path_,  std::fstream::out | std::ofstream::binary);
    }

    void HttpResponseFile::append(const std::vector<std::uint8_t> &chunk) {

      if (outFile_ == nullptr) {
        throw exception("File could not be allocated...");
      }

      if (!outFile_->is_open()) {
        std::string error("File ");
        error += path_;
        error += " could not be openned...";
        throw exception(error.c_str());
      }

      std::copy(chunk.begin(), chunk.end(), std::ostreambuf_iterator<char>(*outFile_));

    }

    void HttpResponseFile::write(std::vector<std::uint8_t> &buffer) {

      buffer.clear();

      std::ifstream inFile;
      inFile.open(path_.c_str(),  std::fstream::in);

      if (!inFile.is_open())
        return ;

      inFile.seekg(0, std::ios::end);
      buffer.reserve(inFile.tellg());
      inFile.seekg(0, std::ios::beg);

      buffer.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

    }

    exception::exception(CURLcode code, std::shared_ptr<HttpResponse> response):
            std::runtime_error(curl_easy_strerror(code)),
            code_(code),
            status_(response ? response->http_status : -1),
            response_(response) {}

    exception::exception(const std::string& error, std::shared_ptr<HttpResponse> response):
            std::runtime_error(error),
            code_(CURLE_RECV_ERROR),
            status_(response ? response->http_status : -1),
            response_(response) {}

}