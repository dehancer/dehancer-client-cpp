//
// Created by denn nevera on 2020-01-01.
//

#include "gtest/gtest.h"
#include <rxcpp/rx.hpp>
#include <string>


TEST(RXCPP_TEST, rxcppproxy) {


  std::cout << std::endl;

  auto proxy = rxcpp::subjects::subject<const int>();

  proxy
          .get_observable()

          .subscribe_on(rxcpp::observe_on_new_thread())

          .subscribe(

                  [](const int index){
                      std::cout << " Index: " << index << std::endl;
                  },

                  [](std::exception_ptr ep){

                      try {
                        std::rethrow_exception(ep);
                      }
                      catch (const std::exception& ex) {
                        std::cout << " Error: " <<  ex.what() << std::endl;
                      }
                  },

                  [](){
                      std::cout << " On complete..." << std::endl;
                  }
          );


  auto observable = rxcpp::observable<>::create<int>(
          [](const rxcpp::subscriber<int>& subscriber){
              for(int i = 0; i<10; ++i) {
                subscriber.on_next(i);
                std::this_thread::sleep_for(std::chrono::milliseconds(270));
              }
              subscriber.on_completed();
          });


  observable

          .filter([proxy](int index){
              if (index > 3 ){
                rxcpp
                ::observable<>
                ::error<const int>(std::runtime_error("Index greater then 3"))
                        .subscribe(proxy.get_subscriber());
                return false;
              }
              return true;
          })

          .subscribe(
                  [proxy](const int index){
                      proxy.get_subscriber().on_next(index);
                  },
                  [proxy](){
                      proxy.get_subscriber().on_completed();
                  }
          );


}