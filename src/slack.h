#pragma once

#include <exception>
#include <expected>
#include <format>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

#include "curl/curl.h"
#include "curlutil.h"
#include "env.h"
#include "oatpp/base/Log.hpp"
#include "oatpp/macro/component.hpp"

class SlackApi {
 public:
  static std::expected<std::string, std::string> post_message(
      std::string message, std::string channel,
      std::optional<std::string> thread_ts = {}) {
    CURLcode result;
    CURL* curl;
    struct curl_slist* slist;

    OATPP_COMPONENT(std::shared_ptr<EnvReader>, env_reader);
    if (!env_reader->get("slack_token").has_value()) {
      return std::unexpected(
          "Backend issue could not get slack_token from env");
    }
    auto slack_token = env_reader->get("slack_token").value();

    slist = nullptr;
    slist = curl_slist_append(
        slist, std::format("Authorization: Bearer {}", slack_token).c_str());
    slist = curl_slist_append(slist, "Content-Type: application/json");

    nlohmann::json payload = {{"channel", channel}, {"markdown_text", message}};
    if (thread_ts.has_value()) {
      payload["thread_ts"] = thread_ts.value();
    }

    auto payload_str = payload.dump();

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(curl, CURLOPT_URL,
                     "https://slack.com/api/chat.postMessage");
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
    curl = nullptr;
    curl_slist_free_all(slist);
    slist = nullptr;

    nlohmann::json out_parsed = nlohmann::json::parse(out_json);

    if (result != CURLE_OK) {
      return std::unexpected("Slack API failed");
    }

    if (!out_parsed["ok"].is_boolean() || out_parsed["ok"] == false) {
      return std::unexpected("Slack API was not ok for message");
    }

    if (!out_parsed["message"].is_object() ||
        !out_parsed["message"]["ts"].is_string()) {
      return std::unexpected("Slack API did not return a timestamp");
    }

    return out_parsed["message"]["ts"].get<std::string>();
  }

  // always returns true or unexpected
  static std::expected<bool, std::string> delete_message(
      std::string channel,
      std::string thread_ts) {
    CURLcode result;
    CURL* curl;
    struct curl_slist* slist;

    OATPP_COMPONENT(std::shared_ptr<EnvReader>, env_reader);
    if (!env_reader->get("slack_token").has_value()) {
      return std::unexpected(
          "Backend issue could not get slack_token from env");
    }
    auto slack_token = env_reader->get("slack_token").value();

    slist = nullptr;
    slist = curl_slist_append(
        slist, std::format("Authorization: Bearer {}", slack_token).c_str());
    slist = curl_slist_append(slist, "Content-Type: application/json");

    nlohmann::json payload = {{"channel", channel}, {"ts", thread_ts}};

    auto payload_str = payload.dump();

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(curl, CURLOPT_URL,
                     "https://slack.com/api/chat.delete");
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
    curl = nullptr;
    curl_slist_free_all(slist);
    slist = nullptr;

    nlohmann::json out_parsed = nlohmann::json::parse(out_json);

    if (result != CURLE_OK) {
      return std::unexpected("Slack API failed");
    }

    if (!out_parsed["ok"].is_boolean() || out_parsed["ok"] == false) {
      return std::unexpected(std::format("Slack API was not ok {}", out_parsed["error"].get<std::string>()));
    }

    return true;
  }
};
