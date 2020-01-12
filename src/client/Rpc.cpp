//
// Created by denn nevera on 2020-01-03.
//

#include <dehancer/client/UrlSession.h>
#include "dehancer/client/Rpc.h"
#include "dehancer/client/UserDefaults.h"

#include <ed25519.hpp>
#include "rxcpp/rx.hpp"

namespace dehancer::network::client {

    namespace rx = rxcpp;

    namespace detail {

        struct RpcClient {

            RpcClient(const std::string &url,
                      const std::string &container_id,
                      const std::string &api_access_token_private,
                      const std::string &api_acces_name,
                      time_t timeout,
                      int    reconnections,
                      time_t reconnection_delay
            ):
                    reconnections_(reconnections),
                    reconnection_delay_(reconnection_delay),
                    session_(url,timeout),
                    user_(dehancer::platform::User::Defaults(container_id)),
                    api_access_token_private_(api_access_token_private),
                    api_acces_name_(api_acces_name),
                    access_token_(),
                    cuid_(user_->get_cuid())
            {
              auto access_pair = ed25519::keys::Pair::FromPrivateKey(api_access_token_private_);
              api_access_pair_.first = access_pair->get_public_key().encode();
              api_access_pair_.second = access_pair->get_private_key().encode();
            }


            promise::Defer request(const std::string &method, const dehancer::json &params) {

              return promise::newPromise([this, method, params](promise::Defer defer) {

                  using namespace dehancer::network;

                  dehancer::json message = {
                          {"jsonrpc", "2.0"},
                          {"method",  method},
                          {"params",  params},
                          {"id",      std::to_string(RpcClient::request_id_++)}
                  };

                  HttpRequest http_request = {
                          {
                                  .headers = {},
                          },
                          .body = message.dump(),
                          .method = HttpRequest::Method::post
                  };

                  session_

                          .request(http_request)

                          .subscribe_on(rxcpp::observe_on_event_loop())

                          .on_error_resume_next([this, http_request](std::exception_ptr ep) {
                              std::this_thread::sleep_for(std::chrono::milliseconds(reconnection_delay_));
                              return session_.request(http_request);
                          })

                          .retry(reconnections_)

                          .subscribe(

                                  [this,defer](const std::shared_ptr<HttpResponse> response){

                                      if (response->state == HttpResponse::State::completed) {
                                        std::string data; response->write(data);
                                        auto result = json::parse(data);

                                        if (result.count("result")>0) {
                                          return defer.resolve(result.at("result"));
                                        }

                                        if (result.count("error")) {
                                          auto error = dehancer::Error(
                                                  dehancer::CommonError::RPC_ERROR,
                                                  result.at("error")["message"]);
                                          return defer.reject(error);
                                        }

                                        auto error = dehancer::Error(
                                                dehancer::CommonError::PARSE_ERROR,
                                                "Response can't be parsed");

                                        return defer.reject(error);
                                      }
                                  },

                                  [this,defer](std::exception_ptr ep) {
                                      try { std::rethrow_exception(ep);}
                                      catch (const exception& e) {
                                        std::string error(e.what());
                                        error.append(" HTTP Status: ");
                                        error.append(std::to_string(e.get_http_status()));
                                        defer.reject(dehancer::Error(dehancer::CommonError::RPC_ERROR, error));
                                      }

                                  }

                          );

              });

            };

            int    reconnections_;
            time_t reconnection_delay_;

            dehancer::network::client::UrlSession session_;
            std::shared_ptr<dehancer::platform::User> user_;

            std::string api_access_token_private_;
            std::string api_acces_name_;
            std::string access_token_;


            std::pair<std::string,std::string> cuid_;
            std::pair<std::string,std::string> api_access_pair_;

            static std::atomic_uint32_t request_id_;
        };

        std::atomic_uint32_t RpcClient::request_id_ = 0;
    }

    Rpc::Rpc(const std::string &url,
             const std::string &container_id,
             const std::string &api_access_token_private,
             const std::string &api_acces_name,
             time_t timeout,
             int    reconnections,
             time_t reconnection_delay
    ) {
      impl_ = std::make_shared<detail::RpcClient>(
              url,
              container_id,
              api_access_token_private,
              api_acces_name,
              timeout,
              reconnections,
              reconnection_delay
      );
    }

    const std::string& Rpc::get_access_token() const {
      return impl_->access_token_;
    }

    promise::Defer Rpc::get_auth_token() const {

      auto digest = ed25519::Digest([this](auto &calculator){
          calculator.append(ed25519::Seed());
          calculator.append(impl_->api_acces_name_);
      });

      auto pair = ed25519::keys::Pair::FromPrivateKey(impl_->api_access_pair_.second);
      auto signature = pair->sign(digest);

      dehancer::json params = {
              {"api-access-token", pair->get_public_key().encode()},
              {"cuid", impl_->cuid_.first},
              {"signature", signature->encode()},
              {"digest", digest.encode()}
      };

      return impl_->request("get-auth-token", params);

    }

    promise::Defer Rpc::get_cuid_state(const std::string &token) const {

      auto digest = ed25519::Digest([this,token](auto &calculator){
          calculator.append(token);
          calculator.append(impl_->cuid_.first);
      });

      auto pair = ed25519::keys::Pair::FromPrivateKey(impl_->cuid_.second);
      auto signature = pair->sign(digest);

      dehancer::json params = {{"cuid", impl_->cuid_.first},{"signature", signature->encode()}};

      return impl_->request("get-cuid-state", params);
    }

    promise::Defer Rpc::login() {
      //return promise::newPromise([this](promise::Defer defer) {

      auto token = impl_->user_->get_defaults("__ACCESS_TOKEN__");

      if (token && !token->empty()) {

        return get_cuid_state(token.value())

                .then([this](const dehancer::json json){
                    return get_auth_token();
                });
      }
      else {

      }

      return promise::newPromise([this](promise::Defer defer) {
          return defer.resolve(this);
      });
          //return defer.resolve(this);
      //});
    }

    const std::string& Rpc::get_url() const { return impl_->session_.get_url(); }

}
