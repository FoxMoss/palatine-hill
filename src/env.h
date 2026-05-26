#pragma once

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
  nlohmann::json data;

 public:
  EnvReader(std::string file_name) {
    if (std::filesystem::exists(file_name)) {
      std::ifstream file_stream(file_name);
      data = nlohmann::json::parse(file_stream);
    } else {
      OATPP_LOGe("Env", "Could not find {}", file_name);
    }
  }

  std::optional<std::string> get(std::string key) {
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

