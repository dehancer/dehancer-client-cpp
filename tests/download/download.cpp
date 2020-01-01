//
// Created by denn nevera on 2019-12-30.
//


#include "gtest/gtest.h"
#include "dehancer/client/UrlSession.h"


TEST(JSON_RPC_CONNECT, UrlSessionTest) {

  namespace client = dehancer::network::client;

  int count = 10;

  std::atomic_uint ref_count = count;

  dehancer::Semaphore semaphore;

  std::cout << std::endl;

  auto session = client::UrlSession("http://localhost:8042/v1/api", 10);

  auto filepath = [](const client::UrlSession& session){
      return std::string("/tmp/download_content.json");
  };


  dehancer::json body =
          R"({"jsonrpc":"2.0",
              "method": "get-film-profile-list",
              "params":{"cuid": "", "signature": "",
              "all": true, "id":""},
              "id":"1"})"_json;

  client::HttpRequest request = {
          {
                  .headers = {},
          },
          .body = body.dump(),
          .method = client::HttpRequest::Method::post
  };

  for (int i = 0; i < count; ++i) {
    session

            .download(filepath,request)

            .subscribe_on(rxcpp::observe_on_event_loop())

            .subscribe(

                    [](const std::shared_ptr<client::HttpResponse> response){

                        std::cout << " Response: ["<< std::this_thread::get_id() <<"]"
                                  << " state: " << response->state << ", progress: " <<  response->progress << std::endl;

                        if (response->state == client::HttpResponse::State::completed) {

                          std::string data;

                          response->write(data);

                          std::cout << " Body["<<response.use_count() << "]: "
                                    << response->content_length << " / " << data.length()
                                    << "\n -- "
                                    << data
                                    << " --"
                                    << std::endl;
                        }

                    },

                    [&semaphore,&session,&ref_count](std::exception_ptr ep){

                        try {
                          std::rethrow_exception(ep);
                        }

                        catch (const client::UrlSession::exception& ex) {
                          std::vector<std::uint8_t > data;

                          if (ex.get_response()) {
                            ex.get_response()->write(data);
                          }

                          std::cout << " Error: "
                                    <<  ex.what() << ": "
                                    << (ex.get_response()
                                    ? std::string(data.begin(),data.end())
                                    : session.get_url())
                                    << std::endl;
                        }

                        catch (const std::exception& ex) {
                          std::cout << " Error: " <<  ex.what() << std::endl;
                        }


                        if (--ref_count == 0) semaphore.signal();

                    },

                    [&semaphore,&ref_count](){
                        std::cout << " On complete ["<< std::this_thread::get_id() <<"]"
                                  << std::endl << std::endl;

                        if (--ref_count == 0) semaphore.signal();
                    }
            );
  }

  std::cout << " ....["<< std::this_thread::get_id() <<"]" << std::endl;
  semaphore.wait();
  std::cout << " Exited." << std::endl;
}
