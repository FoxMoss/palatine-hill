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
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/network/Url.hpp"
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

  ENDPOINT("GET", "/auth/callback", root,
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

      auto payload_str = payload.dump();

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
      localdb->register_access_token(
          me_parsed["identity"]["slack_id"].get<std::string>(),
          out_parsed["access_token"].get<std::string>(), combined_name);

      Status ret_status = Status::CODE_200;
      auto response = createResponse(ret_status, return_body);
      response->putHeader(Header::CONTENT_TYPE, "text/html");
      return response;
    }
  error_response:
    Status ret_status = Status::CODE_303;
    auto response = createResponse(ret_status, out_json);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    response->putHeader("Location", "/error");
    return response;
  }
};

#include OATPP_CODEGEN_END(ApiController)

