#pragma once

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <optional>
#include <print>
#include <string>

#include "nlohmann/json.hpp"
#include "oatpp/base/Log.hpp"

class EnvReader {
 private:
  bool use_env = false;
  nlohmann::json data;

 public:
  EnvReader(std::string file_name) {
    if (std::string(getenv("USE_ENV")) == "") {
      if (std::filesystem::exists(file_name)) {
        std::ifstream file_stream(file_name);
        data = nlohmann::json::parse(file_stream);
      } else {
        OATPP_LOGe("Env", "Could not find {}", file_name);
      }
    } else {
      use_env = true;
    }
  }

  std::optional<std::string> get(std::string key) {
    if (use_env) {
      const char* var = getenv(key.c_str());
      if (var == nullptr) {
        return {};
      }
      return var;
    }
    try {
      if (data[key].is_string()) {
        return data[key].get<std::string>();
      }
      return {};
    } catch (std::exception& e) {
      return {};
    }
  }
};

