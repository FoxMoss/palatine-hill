#pragma once

#include <curl/curl.h>

#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <string>

#include "dbclient.h"
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

#include OATPP_CODEGEN_BEGIN(ApiController)

class ApiController : public oatpp::web::server::api::ApiController {
 public:
  std::string auth_client_id, auth_client_secret;
  ApiController(const std::shared_ptr<oatpp::web::mime::ContentMappers>&
                    apiContentMappers,
                const std::string auth_client_id,
                const std::string auth_client_secret)
      : oatpp::web::server::api::ApiController(apiContentMappers),
        auth_client_id(auth_client_id),
        auth_client_secret(auth_client_secret) {}

  static std::shared_ptr<ApiController> createShared(
      std::string auth_client_id, std::string auth_client_secret,
      OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                      apiContentMappers)  // Inject ContentMappers
  ) {
    return std::make_shared<ApiController>(apiContentMappers, auth_client_id,
                                           auth_client_secret);
  }

  static size_t write_string(char* ptr, size_t, size_t nmemb, void* userdata) {
    auto* user_str = (std::string*)userdata;
    user_str->insert(user_str->end(), ptr, ptr + nmemb);
    return nmemb;
  }

  ENDPOINT("GET", "/auth/callback", verify_auth,
           REQUEST(std::shared_ptr<IncomingRequest>, request),
           QUERY(String, code)) {
    std::string raw_url =
        std::format("http{}://{}{}", PALATINE_PROD ? "s" : "",
                    (std::string)request->getHeader("Host"),
                    request->getStartingLine().path.std_str());

    nlohmann::json payload = {{"client_id", auth_client_id},
                              {"client_secret", auth_client_secret},
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

    std::string out_json;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_json);

    result = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_slist_free_all(slist);
    slist = nullptr;
    if (result != CURLE_OK) {
      goto error_response;
    }

    {
      nlohmann::json out_parsed = nlohmann::json::parse(out_json);

      if (!out_parsed["access_token"].is_string()) {
        goto error_response;
      }

      CURLcode result;
      CURL* curl;
      struct curl_slist* slist = nullptr;
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
      if (submit_params.get("explanation") == "") {
        error = "Must have key \"explanation\"";
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

      res = localdb->create_pitch(
          account_infos[0]->slack_id,
          oatpp::encoding::Url::decode(submit_params.get("title")),
          oatpp::encoding::Url::decode(submit_params.get("explanation")));
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
