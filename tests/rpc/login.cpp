//
// Created by denn nevera on 2020-01-03.
//

#include "gtest/gtest.h"
#include "dehancer/Client.h"
#include "../dotenv/dotenv.h"
#include "../dotenv/dotenv_utils.h"
#include "key_file.h"

TEST(JSON_RPC_CONNECT, LoginTest) {

  auto url = get_dehancerd_url();

  auto client = dehancer::network::client::Rpc(
          url,
          "com.dehancer.test.rpc",
          __DEHANCERD_API_ACCESS_KEY__,
          __DEHANCERD_API_ACCESS_NAME__,
          60
          );


  std::cout << std::endl;

  dehancer::Semaphore semaphore;

  client
          .get_auth_token()

          .then([&](const dehancer::json json){

            std::cout << "result: " << json.dump() << std::endl;

            semaphore.signal();

          })

          .fail([](const dehancer::Error error){

            std::cerr << error << std::endl;

          })

          .finally([&](){
              semaphore.signal();
          });

  std::cout << "waiting....: " << client.get_url() << std::endl;

  semaphore.wait();

  std::cout << "exiting..." << std::endl;

}