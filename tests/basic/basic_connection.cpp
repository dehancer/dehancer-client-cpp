//
// Created by denn nevera on 2019-12-30.
//


#include "gtest/gtest.h"
#include "dehancer/client/UrlSession.hpp"


TEST(JSON_RPC_CONNECT, UrlSessionTest) {

  namespace client = dehancer::network::client;

  dehancer::Semaphore semaphore;

  std::cout << std::endl;

  dehancer::json body =
          R"({"jsonrpc":"2.0",
              "method": "get-film-profile-list",
              "params":{"cuid": "", "signature": "",
              "all": true, "id":""},
              "id":"1"})"_json;

  auto session = client::UrlSession("http://localhost:8042/v1/api", 10);

  client::HttpRequest request = {
          {
                  .headers = {},
                  .body = body.dump()
          },
          .method = client::HttpRequest::Method::post
  };

  session

          .request(request)

          .on_error_resume_next([&](std::exception_ptr ep) {
              std::this_thread::sleep_for(std::chrono::milliseconds(2000));
              return session .request(request);

          })

          .retry(2)

          .subscribe(

                  [](const std::shared_ptr<client::HttpResponse> response){

                      std::cout << " Response: ["<< std::this_thread::get_id() <<"]"
                                << " state: " << response->state << ", progress: " <<  response->progress << std::endl;

                      if (response->state == client::HttpResponse::State::completed) {
                        std::cout << " Body: " << response->body << response->body.size() << std::endl;
                      }

                  },

                  [&semaphore,&session](std::exception_ptr ep){

                      try {
                        std::rethrow_exception(ep);
                      }

                      catch (const client::UrlSession::exception& ex) {
                        std::cout << " Error: "
                        <<  ex.what() << ": " << (ex.get_response() ? ex.get_response()->body : session.get_url())
                        << std::endl;
                      }

                      catch (const std::exception& ex) {
                        std::cout << " Error: " <<  ex.what() << std::endl;
                      }

                      semaphore.signal();
                  },

                  [&semaphore](){
                      std::cout << " On complete ["<< std::this_thread::get_id() <<"]"
                                << std::endl << std::endl;
                      semaphore.signal();
                  }
          );

  std::cout << " ....["<< std::this_thread::get_id() <<"]" << std::endl;
  semaphore.wait();
  std::cout << " Exited." << std::endl;

}
