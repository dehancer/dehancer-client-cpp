//
// Created by denn nevera on 2020-01-03.
//

#include "gtest/gtest.h"
#include "dehancer/Client.h"
#include "../dotenv/dotenv.h"
#include "../dotenv/dotenv_utils.h"
#include <random>
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

  int count = 1;
  dehancer::Semaphore semaphore(-(count-1));

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(100, 300);

  for (int i = 0; i < count; ++i) {

    auto duration = std::chrono::milliseconds(dis(gen));

    client
            .login()

            .then([duration](dehancer::network::client::Rpc *rpc) {

                std::this_thread::sleep_for(duration);

                return rpc->get_auth_token();
            })

            .then([&](const dehancer::json& json) {

                std::this_thread::sleep_for(duration);

                std::cout << "auth result: " << json.dump() << std::endl;

            })

            .fail([](const dehancer::Error& error) {

                std::cerr << error << std::endl;

            })

            .finally([&]() {
                semaphore.signal();
            });
  }

  std::cout << "waiting....: " << client.get_url() << std::endl;

  semaphore.wait();

  std::cout << "exiting..." << std::endl;

}