//
// Created by denn nevera on 2019-12-30.
//

#include "dehancer/client/UrlSession.h"
#include <algorithm>
#include <regex>

namespace dehancer::network::client {

    namespace detail {

        //
        // https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
        //
        static const std::regex __http_header_regex_(R"((.+?):[\t ]*(.+))");

        static std::string url_encode(std::string s);

        struct Response {
            std::size_t received_size = 0;
            rx::subjects::subject<std::vector<std::uint8_t>> body_progress;
            rx::subjects::subject<std::string> header_progress;
            CURL *curl;
            ~Response() {
              body_progress.get_subscription().unsubscribe();
              header_progress.get_subscription().unsubscribe();
            }
        };

        static size_t header_handler(char *buffer, size_t size,
                                     size_t nitems, Response *response)
        {

          int rsize = size * nitems;
          std::string chunk(buffer,rsize);
          response->header_progress.get_subscriber().on_next(chunk);

          return rsize;
        }

        static size_t write_handler(char *ptr, size_t size, size_t nmemb, Response *response) {
          int rsize = size * nmemb;

          std::vector<std::uint8_t> chunk(ptr, ptr+rsize);
          response->received_size += rsize;
          response->body_progress.get_subscriber().on_next(chunk);

          return rsize;
        }

        struct Session {

            ~Session() {};

            Session(const std::string& url, std::time_t timeout):
                    url(url_encode(url)),timeout(timeout)
            {};

            const std::string url;
            std::time_t       timeout;


            rx::observable<std::shared_ptr<HttpResponse>> request(const HttpRequest &request, std::shared_ptr<HttpResponse>&& response) const {

              return rx::observable<>::create<std::shared_ptr<HttpResponse>>(

                      [this, &request, response](rx::subscriber<std::shared_ptr<HttpResponse>>& subscriber) {

                          if (!response) {
                            rx
                            ::observable<>
                            ::error<std::shared_ptr<HttpResponse>>(std::runtime_error("Could not allocate response data..."))
                                    .subscribe(subscriber);
                            return;
                          }

                          CURL *curl = curl_easy_init();

                          if (!curl) {

                            rx
                            ::observable<>
                            ::error<std::shared_ptr<HttpResponse>>(std::runtime_error("Could not allocate curl session..."))
                                    .subscribe(subscriber);

                            return;

                          }

                          auto curl_response = detail::Response();

                          curl_response.curl = curl;

                          curl_response
                                  .header_progress
                                  .get_observable().subscribe(
                                          [&response, curl, &subscriber](std::string chunk) {
                                              try {

                                                if (response->state == HttpResponse::State::undefined) {
                                                  response->progress = 0;
                                                  response->state = HttpResponse::State::in_progress;
                                                  subscriber.on_next(response);
                                                }

                                                auto end = chunk.begin();
                                                if (chunk.size()>=2 && *(end) == '\r' && *(end+1) == '\n') {
                                                  // end of header
                                                  double cl;
                                                  curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &cl);
                                                  response->content_length = static_cast<std::size_t >(cl);
                                                }

                                                std::cmatch what;
                                                if (chunk.size()>=2 && std::regex_match(chunk.c_str(), chunk.c_str() + chunk.size()-2, what, detail::__http_header_regex_)) {
                                                  if (what.size() == 3 ) {
                                                    std::string header = std::string(what[1]); // 0
                                                    std::string header_value = std::string(what[2]); // 1
                                                    response->headers[header] = header_value;
                                                  }
                                                }
                                              }
                                              catch (...){}
                                          }
                                  );

                          curl_response
                                  .body_progress
                                  .get_observable()
                                  .subscribe(
                                          [&response, &curl_response, &subscriber]
                                                  (const std::vector<std::uint8_t> &chunk) {

                                              try {

                                                response->append(chunk);

                                                if (response->content_length > 0) {
                                                  float p = float(curl_response.received_size) /
                                                            float(response->content_length) * 100.0;
                                                  response->progress = static_cast<short>(p);
                                                }
                                                subscriber.on_next(response);
                                              }

                                              catch (const UrlSession::exception &e) {
                                                rx::observable<>::error<std::shared_ptr<HttpResponse>>(e).subscribe(subscriber);
                                              }

                                          }

                                  );


                          subscriber.on_next(response);

                          curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

                          if (request.method == HttpRequest::Method::post) {
                            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) request.body.size());
                            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.data());
                          }

                          curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

                          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, detail::write_handler);
                          curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curl_response);

                          curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, detail::header_handler);
                          curl_easy_setopt(curl, CURLOPT_HEADERDATA, &curl_response);

                          CURLcode res = curl_easy_perform(curl);

                          curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->http_status);

                          ///
                          /// Check for errors
                          ///
                          if (res != CURLE_OK) {
                            rx::observable<>::error<std::shared_ptr<HttpResponse>>(UrlSession::exception(res)).subscribe(subscriber);
                          }

                          else if (response->http_status>=400) {
                            rx::observable<>
                            ::error<std::shared_ptr<HttpResponse>>(UrlSession::exception("Http response error...", response))
                                    .subscribe(subscriber);
                          }

                          else {

                            response->state = HttpResponse::State::completed;
                            response->progress = 100;
                            subscriber.on_next(response);

                            subscriber.on_completed();
                          }

                          curl_easy_cleanup(curl);
                      }
              );
            }
        };

    }

    HttpResponse::~HttpResponse() {}

    void HttpResponseMessage::append(const std::vector<std::uint8_t> &chunk) {
      body_.insert(body_.end(), chunk.begin(), chunk.end());
    }

    const std::vector<std::uint8_t>& HttpResponseMessage::data() const {
      return body_;
    }

    HttpResponseFile::HttpResponseFile(const std::string_view &file):
    path_(file),outFile_(new std::ofstream) {
      outFile_->open(path_,  std::fstream::out | std::ofstream::binary);
    }

    void HttpResponseFile::append(const std::vector<std::uint8_t> &chunk) {

      if (outFile_ == nullptr) {
        throw UrlSession::exception("File could not be allocated...");
      }

      if (!outFile_->is_open()) {
        std::string error("File ");
        error += path_;
        error += " could not be openned...";
        throw UrlSession::exception(error.c_str());
      }

      std::copy(chunk.begin(), chunk.end(), std::ostreambuf_iterator<char>(*outFile_));

    }

    const std::vector<std::uint8_t>& HttpResponseFile::data() const {
      return body_;
    }

    UrlSession::exception::exception(CURLcode code, std::shared_ptr<HttpResponse> response):
            std::runtime_error(curl_easy_strerror(code)), code_(code), response_(response) {}

    UrlSession::exception::exception(const std::string& error, std::shared_ptr<HttpResponse> response):
            std::runtime_error(error), code_(CURLE_RECV_ERROR), response_(response) {}

    UrlSession::UrlSession(const std::string &url, std::time_t timeout):
            session_(new detail::Session(url,timeout)){}


    const std::string& UrlSession::get_url() const {
      return session_->url;
    }

    rx::observable<std::shared_ptr<HttpResponse>> UrlSession::request(const HttpRequest &request) const {
      auto response = std::make_shared<HttpResponseMessage>();
      return session_->request(request, std::move(response));
    }

    rxcpp::observable<std::shared_ptr<HttpResponse>> UrlSession::download(
            const PathHandler &handler,
            const HttpRequest& request) const {

      try {

        std::string_view filepath = handler(*this);
        auto response = std::make_shared<HttpResponseFile>(filepath);

        return session_->request(request, std::move(response));

      }

      catch (const std::exception& e) {
        return rx::observable<>::create<std::shared_ptr<HttpResponse>>(
                [this, &e](rx::subscriber<std::shared_ptr<HttpResponse>>& subscriber) {
                    rx::observable<>::error<std::shared_ptr<HttpResponse>>(e).subscribe(subscriber);
                }
        );
      }
    }


    static void hexchar(unsigned char c, unsigned char &hex1, unsigned char &hex2)
    {
      hex1 = c / 16;
      hex2 = c % 16;
      hex1 += hex1 <= 9 ? '0' : 'a' - 10;
      hex2 += hex2 <= 9 ? '0' : 'a' - 10;
    }

    namespace detail {

        static std::string url_encode(std::string s) {
          const char *str = s.c_str();
          std::vector<char> v(s.size());
          v.clear();
          for (size_t i = 0, l = s.size(); i < l; i++) {
            char c = str[i];
            if ((c >= '0' && c <= '9') ||
                (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                c == '-' || c == '_' || c == '.' || c == '!' || c == '~' ||
                c == '*' || c == '\'' || c == '(' || c == ')' || c == '/' || c == ':' ||
                c == '?' || c == '&' || c == '='
                    ) {
              v.push_back(c);
            } else if (c == ' ') {
              v.push_back('+');
            } else {
              v.push_back('%');
              unsigned char d1, d2;
              hexchar(c, d1, d2);
              v.push_back(d1);
              v.push_back(d2);
            }
          }

          return std::string(v.cbegin(), v.cend());
        }
    }
}