#pragma once

#include <curl/curl.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <ranges>
#include <string>

#include "curlutil.h"
#include "dbclient.h"
#include "env.h"
#include "nlohmann/json_fwd.hpp"
#include "oatpp/Types.hpp"
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

  ENDPOINT("POST", "/auth/link_hackatime", link_hackatime,
           REQUEST(std::shared_ptr<IncomingRequest>, request)) {
    std::string out_json;  // error output message
    {
      auto body = request->readBodyToString();
      nlohmann::json parsed = nlohmann::json::parse((std::string)body);

      std::string app_token = parsed["app_token"].get<std::string>();
      std::string code = parsed["code"].get<std::string>();

      std::string raw_url = std::format(
          "http{}://{}{}", PALATINE_PROD ? "s" : "",
          (std::string)request->getHeader("Host"), "/auth/hackatime_callback");

      if (!env_reader->get("hackatime_id").has_value()) {
        out_json = "Backend error: .env.json did not have hackatime_id";
        goto error_response;
      }

      if (!env_reader->get("hackatime_secret").has_value()) {
        out_json = "Backend error: .env.json did not have hackatime_secret";
        goto error_response;
      }

      std::string payload_str = std::format(
          "client_id={}&client_secret={}&redirect_uri={}&code={}&grant_type="
          "authorization_code",
          (std::string)oatpp::encoding::Url::encode(
              env_reader->get("hackatime_id").value(),
              oatpp::encoding::Url::Config()),
          (std::string)oatpp::encoding::Url::encode(
              env_reader->get("hackatime_secret").value(),
              oatpp::encoding::Url::Config()),
          (std::string)oatpp::encoding::Url::encode(
              raw_url, oatpp::encoding::Url::Config()),
          (std::string)oatpp::encoding::Url::encode(
              code, oatpp::encoding::Url::Config()));

      CURLcode result;
      CURL* curl;
      struct curl_slist* slist = nullptr;
      slist = curl_slist_append(
          slist, "Content-Type: application/x-www-form-urlencoded");

      curl = curl_easy_init();
      curl_easy_setopt(curl, CURLOPT_URL,
                       "https://hackatime.hackclub.com/oauth/token");
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

      std::println("{}", out_json);
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
      curl_easy_setopt(
          curl, CURLOPT_URL,
          "https://hackatime.hackclub.com/api/v1/authenticated/me");
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

      OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);
      auto res = localdb->link_hackatime(
          out_parsed["access_token"].get<std::string>(), app_token);

      if (!res->isSuccess()) {
        out_json = res->getErrorMessage();
        goto error_response;
      }

      Status ret_status = Status::CODE_200;
      auto response = createResponse(ret_status, "ok");
      response->putHeader(Header::CONTENT_TYPE, "text/html");
      response->putHeader("Location", "/setup");

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

  ENDPOINT("GET", "/auth/hackatime_callback", verify_hackatime,
           REQUEST(std::shared_ptr<IncomingRequest>, request),
           QUERY(String, code)) {
    std::string out_json;  // error output message

    {
      std::string html_body = std::format(R"html(
<script>
  const app_token = localStorage.getItem('access_token');
  const code = '{}';

  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/auth/link_hackatime', true);
  xhr.setRequestHeader('Content-Type', 'application/json');
  xhr.onload = function() {{
    if (xhr.status == 200) {{
      window.location.href = '/setup';
    }} else if (xhr.status >= 300 && xhr.status < 400) {{
      window.location.href = xhr.getResponseHeader('Location') || '/error?error=Hackatime failed';
    }} else {{
      window.location.href = '/error?error=' + encodeURIComponent(xhr.responseText);
    }}
  }};
  xhr.onerror = function() {{
    window.location.href = '/error?error=network_error';
  }};
  xhr.send(JSON.stringify({{
    app_token: app_token,
    code: code 
  }}));
</script>
)html",
                                          (std::string)code);

      Status ret_status = Status::CODE_200;
      auto response = createResponse(ret_status, html_body);
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
          R"(<script>localStorage["access_token"]={}; localStorage["name"]={}; window.location.assign("/setup");</script>)",
          nlohmann::json(nlohmann::json::string_t(
                             out_parsed["access_token"].get<std::string>()))
              .dump(),
          nlohmann::json(nlohmann::json::string_t(combined_name)).dump());

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

  static std::expected<std::string, std::string> grab_message_thread(
      std::string slack_id, int32_t project_id) {
    OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);
    auto res = localdb->get_vote_ts(slack_id, project_id);

    if (!res->isSuccess()) {
      return std::unexpected(res->getErrorMessage());
    }

    auto timestamps =
        res->fetch<oatpp::Vector<oatpp::Object<VoteMessageTimestampDTO>>>(1);

    if (timestamps->size() != 1) {
      return std::unexpected("Account info rows turned out unexpected");
    }

    return timestamps[0]->message_timestamp;
  }
  static std::expected<std::string, std::string> grab_slack_id(
      std::string access_token) {
    OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);
    auto res = localdb->get_account_from_access_token(access_token);

    if (!res->isSuccess()) {
      return std::unexpected(res->getErrorMessage());
    }

    auto account_infos =
        res->fetch<oatpp::Vector<oatpp::Object<AccountInfoDto>>>(1);

    if (account_infos->size() != 1) {
      return std::unexpected("Account info rows turned out unexpected");
    }

    return account_infos[0]->slack_id;
  }

  static std::expected<std::string, std::string> grab_project(int32_t id) {
    OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);
    auto res = localdb->get_project_from_id(id);

    if (!res->isSuccess()) {
      return std::unexpected(res->getErrorMessage());
    }

    auto post_infos = res->fetch<oatpp::Vector<oatpp::Object<PostDto>>>(1);

    if (post_infos->size() != 1) {
      return std::unexpected("Post info rows turned out unexpected");
    }

    return post_infos[0]->pitch_timestamp;
  }

  ENDPOINT("POST", "/api/v1/set_nickname", set_nickname,
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
      if (submit_params.get("nickname") == "") {
        error = "Must have key \"nickname\"";
        goto unhandled_error;
      }

      if (submit_params.get("nickname")->size() > 20) {
        error = "Nickname was longer then 20 characters";
        goto unhandled_error;
      }

      OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);

      auto slack_id =
          grab_slack_id((std::string)submit_params.get("access_token"));
      if (!slack_id.has_value()) {
        error = slack_id.error();
        goto unhandled_error;
      }

      auto nickname =
          oatpp::encoding::Url::decode(submit_params.get("nickname"));

      localdb->set_nickname(slack_id.value(), nickname);

      std::string return_body = std::format(
          R"(<script>localStorage["name"]={}; window.location.assign("/dashboard");</script>)",
          nlohmann::json(nlohmann::json::string_t(nickname)).dump());

      Status ret_status = Status::CODE_200;
      auto response = createResponse(ret_status, return_body);

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

      auto slack_id =
          grab_slack_id((std::string)submit_params.get("access_token"));
      if (!slack_id.has_value()) {
        error = slack_id.error();
        goto unhandled_error;
      }

      auto title = oatpp::encoding::Url::decode(submit_params.get("title"));
      auto explanation =
          oatpp::encoding::Url::decode(submit_params.get("explanation"));

      auto thread_posted_timestamp = SlackApi::post_message(
          std::format("<@{}> Pitched: **{}**\n\nFollow the thread for updates.",
                      slack_id.value(), (std::string)title),
          "C0B697HHCE6");

      if (!thread_posted_timestamp.has_value()) {
        error = thread_posted_timestamp.error();
        goto unhandled_error;
      }

      auto explanation_msg = SlackApi::post_message(
          explanation, "C0B697HHCE6", thread_posted_timestamp.value());
      if (!explanation_msg.has_value()) {
        error = explanation_msg.error();
        goto unhandled_error;
      }

      auto res = localdb->create_pitch(slack_id.value(), title, explanation,
                                       thread_posted_timestamp.value());
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

      std::vector<std::pair<double, oatpp::Object<PostDto>>> ranked;
      for (auto post : *posts) {
        uint64_t unix_epoch =
            strtoull(post->time_created->c_str(), nullptr, 10);
        constexpr double hours_in_seconds = 60.0 * 60.0;

        double hours_ago =
            (double)(time(nullptr) - unix_epoch) / hours_in_seconds;

        auto score = (post->vote_count - 1.0) / std::pow(hours_ago + 2.0, 1.8);
        ranked.emplace_back(score, post);
      }

      std::ranges::sort(ranked,
                        [](std::pair<double, oatpp::Object<PostDto>>& a,
                           std::pair<double, oatpp::Object<PostDto>>& b) {
                          return a.first < b.first;
                        });

      oatpp::Vector<oatpp::Object<PostDto>> ranked_posts =
          oatpp::Vector<oatpp::Object<PostDto>>::createShared();

      for (auto& post_iter : std::views::reverse(ranked)) {
        ranked_posts->emplace_back(post_iter.second);
      }

      Status ret_status = Status::CODE_200;

      OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                      contentmappers);

      auto response = ResponseFactory::createResponse(
          ret_status, ranked_posts, contentmappers->getDefaultMapper());
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

  ENDPOINT("POST", "/api/v1/unvote", unvote,
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
      if (submit_params.get("project_id") == "") {
        error = "Must have key \"project_id\"";
        goto unhandled_error;
      }

      auto slack_id =
          grab_slack_id((std::string)submit_params.get("access_token"));
      if (!slack_id.has_value()) {
        error = slack_id.error();
        goto unhandled_error;
      }

      int32_t project_id = std::stol(submit_params.get("project_id"));

      OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);

      auto ts = grab_message_thread(slack_id.value(), project_id);
      if (!ts.has_value()) {
        error = ts.error();
        goto unhandled_error;
      }

      auto err = SlackApi::delete_message("C0B697HHCE6", ts.value());
      if (!err.has_value()) {
        error = err.error();
        goto unhandled_error;
      }

      localdb->delete_vote(slack_id.value(), project_id);

      Status ret_status = Status::CODE_200;
      auto response = createResponse(ret_status, "yep!");
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

  ENDPOINT("POST", "/api/v1/upvote", upvote,
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
      if (submit_params.get("project_id") == "") {
        error = "Must have key \"project_id\"";
        goto unhandled_error;
      }

      auto slack_id =
          grab_slack_id((std::string)submit_params.get("access_token"));
      if (!slack_id.has_value()) {
        error = slack_id.error();
        goto unhandled_error;
      }

      int32_t project_id = std::stol(submit_params.get("project_id"));

      auto project_ts = grab_project(project_id);
      if (!project_ts.has_value()) {
        error = project_ts.error();
        goto unhandled_error;
      }

      auto like_msg =
          SlackApi::post_message(std::format("<@{}> Liked", slack_id.value()),
                                 "C0B697HHCE6", project_ts.value());

      if (!like_msg.has_value()) {
        error = like_msg.error();
        goto unhandled_error;
      }

      OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);
      localdb->create_vote(slack_id.value(), project_id, like_msg.value());

      Status ret_status = Status::CODE_200;
      auto response = createResponse(ret_status, "yep!");
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

  ENDPOINT("POST", "/api/v1/about_me", about_me,
           REQUEST(std::shared_ptr<IncomingRequest>, request)) {
    std::string error;  // error output message

    {
      auto body = request->readBodyToString();
      nlohmann::json parsed = nlohmann::json::parse((std::string)body);

      OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);
      std::string access_token = parsed["access_token"].get<std::string>();
      auto res = localdb->get_account_from_access_token(access_token);
      if (!res->isSuccess()) {
        error = res->getErrorMessage();
        goto unhandled_error;
      }

      auto account_info =
          res->fetch<oatpp::Vector<oatpp::Object<AccountInfoDto>>>(1);

      OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                      contentmappers);

      Status ret_status = Status::CODE_200;
      auto response = ResponseFactory::createResponse(
          ret_status, account_info, contentmappers->getDefaultMapper());
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
  ENDPOINT("POST", "/api/v1/my_votes", my_votes,
           REQUEST(std::shared_ptr<IncomingRequest>, request)) {
    std::string error;  // error output message

    {
      auto body = request->readBodyToString();
      nlohmann::json parsed = nlohmann::json::parse((std::string)body);

      OATPP_COMPONENT(std::shared_ptr<LocalDb>, localdb);
      std::string access_token = parsed["access_token"].get<std::string>();

      auto slack_id = grab_slack_id(access_token);
      if (!slack_id.has_value()) {
        error = slack_id.error();
        goto unhandled_error;
      }

      auto res = localdb->get_my_votes(slack_id.value());

      if (!res->isSuccess()) {
        error = res->getErrorMessage();
        goto unhandled_error;
      }

      auto votes = res->fetch<oatpp::Vector<oatpp::Object<VoteDto>>>(-1);

      OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                      contentmappers);

      Status ret_status = Status::CODE_200;
      auto response = ResponseFactory::createResponse(
          ret_status, votes, contentmappers->getDefaultMapper());
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
