#pragma once

#include <curl/curl.h>

#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <string>

#include "curlutil.h"
#include "dbclient.h"
#include "env.h"
#include "nlohmann/json_fwd.hpp"
#include "oatpp/base/Log.hpp"
#include "oatpp/data/mapping/ObjectMapper.hpp"
#include "oatpp/encoding/Url.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/network/Url.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "slack.h"

#include OATPP_CODEGEN_BEGIN(ApiController)

class ApiController : public oatpp::web::server::api::ApiController {
  const std::shared_ptr<EnvReader> env_reader;

 public:
  ApiController(const std::shared_ptr<oatpp::web::mime::ContentMappers>&
                    api_content_mappers,
                const std::shared_ptr<EnvReader> env_reader)
      : oatpp::web::server::api::ApiController(api_content_mappers),
        env_reader(env_reader) {}

  static std::shared_ptr<ApiController> createShared(
      OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                      api_content_mappers),
      OATPP_COMPONENT(std::shared_ptr<EnvReader>, env_reader)) {
    return std::make_shared<ApiController>(api_content_mappers, env_reader);
  }

  ENDPOINT("GET", "/auth/callback", verify_auth,
           REQUEST(std::shared_ptr<IncomingRequest>, request),
           QUERY(String, code)) {
    std::string out_json;  // error output message

    {
      std::string raw_url =
          std::format("http{}://{}{}", PALATINE_PROD ? "s" : "",
                      (std::string)request->getHeader("Host"),
                      request->getStartingLine().path.std_str());

      if (!env_reader->get("hca_client_id").has_value()) {
        out_json = "Backend error: .env.json did not have hca_client_id";
        goto error_response;
      }

      if (!env_reader->get("hca_client_secret").has_value()) {
        out_json = "Backend error: .env.json did not have hca_client_secret";
        goto error_response;
      }

      nlohmann::json payload = {
          {"client_id", env_reader->get("hca_client_id").value()},
          {"client_secret", env_reader->get("hca_client_secret").value()},
          {"redirect_uri", raw_url},
          {"code", code},
          {"grant_type", "authorization_code"}};
      auto payload_str = payload.dump();

      CURLcode result;
      CURL* curl;
      struct curl_slist* slist = nullptr;
      slist = curl_slist_append(slist, "Content-Type: application/json");

      curl = curl_easy_init();
      curl_easy_setopt(curl, CURLOPT_URL,
                       "https://auth.hackclub.com/oauth/token");
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_str.c_str());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE,
                       (curl_off_t)payload_str.size());
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_string);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_json);

      result = curl_easy_perform(curl);

      curl_easy_cleanup(curl);
      curl_slist_free_all(slist);
      slist = nullptr;
      if (result != CURLE_OK) {
        goto error_response;
      }

      nlohmann::json out_parsed = nlohmann::json::parse(out_json);

      if (!out_parsed["access_token"].is_string()) {
        goto error_response;
      }

      slist = curl_slist_append(slist, "Content-Type: application/json");
      slist = curl_slist_append(
          slist, std::format("Authorization: Bearer {}",
                             out_parsed["access_token"].get<std::string>())
                     .c_str());

      curl = curl_easy_init();
      curl_easy_setopt(curl, CURLOPT_URL,
                       "https://auth.hackclub.com/api/v1/me");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

      out_json = "";
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_string);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_json);

      result = curl_easy_perform(curl);

      curl_easy_cleanup(curl);
      curl_slist_free_all(slist);
      slist = nullptr;

      if (result != CURLE_OK) {
        goto error_response;
      }

      nlohmann::json me_parsed = nlohmann::json::parse(out_json);

      std::string first_name = "Placeholder";
      std::string last_name = "Johnson";

      if (me_parsed["identity"].is_object()) {
        if (me_parsed["identity"]["first_name"].is_string()) {
          first_name = me_parsed["identity"]["first_name"].get<std::string>();
        }

        if (me_parsed["identity"]["last_name"].is_string()) {
          last_name = me_parsed["identity"]["last_name"].get<std::string>();
        }
      }

      if (!me_parsed["identity"]["slack_id"].is_string()) {
        goto error_response;
      }

      std::string combined_name = std::format("{} {}", first_name, last_name);

      std::string return_body = std::format(
          R"(<script>localStorage["access_token"]="{}"; localStorage["name"]="{}"; window.location.assign("/dashboard");</script>)",
          out_parsed["access_token"].get<std::string>(), combined_name);

      OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);
      auto res = localdb->register_access_token(
          me_parsed["identity"]["slack_id"].get<std::string>(),
          out_parsed["access_token"].get<std::string>(), combined_name);

      if (!res->isSuccess()) {
        out_json = res->getErrorMessage();
        goto error_response;
      }

      Status ret_status = Status::CODE_200;
      auto response = createResponse(ret_status, return_body);
      response->putHeader(Header::CONTENT_TYPE, "text/html");
      return response;
    }
  error_response:
    Status ret_status = Status::CODE_303;
    auto response = createResponse(ret_status, out_json);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    response->putHeader(
        "Location", std::format("/error?error={}",
                                (std::string)oatpp::encoding::Url::encode(
                                    out_json, oatpp::encoding::Url::Config())));
    return response;
  }

  const size_t max_title_len = 256;
  const size_t max_explanation_len = 12000;  // max slack message len

  ENDPOINT("POST", "/api/v1/pitch_submit", pitch_submit,
           REQUEST(std::shared_ptr<IncomingRequest>, request)) {
    std::string error = "Unknown error";

    {
      auto submit_str =
          "?" + request->getBodyDecoder()->decodeToString(
                    request->getHeaders(), request->getBodyStream().get(),
                    request->getConnection().get());
      auto submit_params =
          oatpp::network::Url::Parser::parseQueryParams(submit_str);

      if (submit_params.get("access_token") == "") {
        error = "Must have key \"access_token\"";
        goto unhandled_error;
      }
      if (submit_params.get("title") == "") {
        error = "Must have key \"title\"";
        goto unhandled_error;
      }

      if (submit_params.get("title")->size() > max_title_len) {
        error = "Title was longer then 256 characters";
        goto unhandled_error;
      }

      if (submit_params.get("explanation") == "") {
        error = "Must have key \"explanation\"";
        goto unhandled_error;
      }
      if (submit_params.get("explanation")->size() > max_explanation_len) {
        error = "Explanation was longer then 12000 characters";
        goto unhandled_error;
      }

      OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);

      auto res = localdb->get_account_from_access_token(
          submit_params.get("access_token"));

      if (!res->isSuccess()) {
        error = res->getErrorMessage();
        goto unhandled_error;
      }

      auto account_infos =
          res->fetch<oatpp::Vector<oatpp::Object<AccountInfoDto>>>(1);

      if (account_infos->size() != 1) {
        error = "Account info rows turned out unexpected";
        goto unhandled_error;
      }

      auto title = oatpp::encoding::Url::decode(submit_params.get("title"));
      auto explanation =
          oatpp::encoding::Url::decode(submit_params.get("explanation"));

      OATPP_COMPONENT(std::shared_ptr<SlackApi>, slack_api);
      auto thread_posted_timestamp = slack_api->post_message(
          std::format("<@{}> Pitched: **{}**\n\nFollow the thread for updates.",
                      (std::string)account_infos[0]->slack_id,
                      (std::string)title),
          "C0B697HHCE6");

      if (!thread_posted_timestamp.has_value()) {
        error = thread_posted_timestamp.error();
        goto unhandled_error;
      }

      auto explanation_msg = slack_api->post_message(
          explanation, "C0B697HHCE6", thread_posted_timestamp.value());
      if (!explanation_msg.has_value()) {
        error = explanation_msg.error();
        goto unhandled_error;
      }

      res = localdb->create_pitch(account_infos[0]->slack_id, title,
                                  explanation, thread_posted_timestamp.value());
      if (!res->isSuccess()) {
        error = res->getErrorMessage();
        goto unhandled_error;
      }

      Status ret_status = Status::CODE_303;
      auto response = createResponse(ret_status, submit_params.get("title"));
      response->putHeader("Location", "/dashboard");

      response->putHeader(Header::CONTENT_TYPE, "text/html");

      return response;
    }

  unhandled_error:
    Status ret_status = Status::CODE_303;
    auto response = createResponse(ret_status, "");
    response->putHeader(Header::CONTENT_TYPE, "text/html");

    response->putHeader(
        "Location", std::format("/error?error={}",
                                (std::string)oatpp::encoding::Url::encode(
                                    error, oatpp::encoding::Url::Config())));
    return response;
  }

  ENDPOINT("GET", "/api/v1/dashboard", get_dashboard,
           REQUEST(std::shared_ptr<IncomingRequest>, request)) {
    std::string error = "Unknown error";

    {
      OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);

      auto res = localdb->get_pitches();

      if (!res->isSuccess()) {
        error = res->getErrorMessage();
        goto unhandled_error;
      }

      auto posts = res->fetch<oatpp::Vector<oatpp::Object<PostDto>>>(-1);
      Status ret_status = Status::CODE_200;

      OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                      contentmappers);

      auto response = ResponseFactory::createResponse(
          ret_status, posts, contentmappers->getDefaultMapper());
      response->putHeader(Header::CONTENT_TYPE, "application/json");
      return response;
    }

  unhandled_error:
    Status ret_status = Status::CODE_303;
    auto response = createResponse(ret_status, "");
    response->putHeader(Header::CONTENT_TYPE, "text/html");

    response->putHeader(
        "Location", std::format("/error?error={}",
                                (std::string)oatpp::encoding::Url::encode(
                                    error, oatpp::encoding::Url::Config())));
    return response;
  }
};

#include OATPP_CODEGEN_END(ApiController)
