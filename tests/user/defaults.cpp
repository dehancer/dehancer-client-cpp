//
// Created by denn nevera on 2020-01-03.
//

#include "dehancer/Client.h"
#include "gtest/gtest.h"

TEST(USER, UserTest) {

  auto user = dehancer::platform::User::Defaults("com.dehancer.test");

  std::cout << std::endl;

  std::cout << std::endl;
  std::cout << "User id        : " << user->get_id() << std::endl;
  //std::cout << "User cuiid     : " << user.get_cuid().first << " | " << user.get_cuid().second << std::endl;
  std::cout << "User name      : " << user->get_name() << std::endl;
  std::cout << "User storage_   : " << user->get_storage_dir() << std::endl;
  std::cout << "User pref file : " << user->get_preference_path() << std::endl;

  auto some_data = user->get_defaults("user-key");

  EXPECT_TRUE(some_data);

  if (!some_data) {
    std::cout << "Error: " << some_data.error().message() << std::endl;
    return;
  }

  std::cout << "User key: " << some_data->dump() << std::endl;

  int count = 42;
  if (some_data->count("data-key")>0){
    count = some_data->at("data-key").get<int>()+1;
    some_data.value()["data-key"] = count;
  }
  else {
    some_data.value() = {{"data-key", count}};
  }

  std::cout << "User key set: " << user->set_defaults(*some_data, "user-key");

  some_data = user->get_defaults("user-key");

  EXPECT_TRUE(some_data.value()["data-key"] == count);
}