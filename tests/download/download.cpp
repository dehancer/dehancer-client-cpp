//
// Created by denn nevera on 2019-12-30.
//

#include "gtest/gtest.h"
#include "dehancer/Client.h"


TEST(JSON_RPC_CONNECT, UrlSessionTest) {

  namespace net = dehancer::network;
  namespace client = dehancer::network::client;

  int count = 1;

  std::atomic_uint ref_count = count;

  dehancer::Semaphore semaphore;

  std::cout << std::endl;

  auto session = client::UrlSession("http://127.0.0.1:8042/v1/download/film/agfa-scala-200x/revision:1.mlut", 10);

  auto filepath = [](const client::UrlSession& session){
      return std::string("/tmp/download_content.xmp");
  };


  for (int i = 0; i < count; ++i) {
    session

            .download(filepath)

            .subscribe_on(rxcpp::observe_on_event_loop())

            .on_error_resume_next([&](std::exception_ptr ep) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                return session .download(filepath);
            })

            .retry(10)

            .subscribe(

                    [](const std::shared_ptr<net::HttpResponse> response){

                        std::cout << " Response: ["<< std::this_thread::get_id() <<"]"
                                  << " state: " << response->state
                                  << ", progress: " <<  response->progress
                                  << " / " <<  response->received_length << "(bytes)"
                                  << std::endl;

                        if (response->state == net::HttpResponse::State::completed) {

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

                        catch (const net::exception& ex) {
                          std::vector<std::uint8_t > data;

                          if (ex.get_response()) {
                            ex.get_response()->write(data);
                          }

                          std::cout << " Error["<<ex.get_http_status()<<"]: "
                                    <<  ex.what() << ": " << ex.get_http_status() << " => "
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
