#pragma once

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <print>

#include "oatpp/base/Log.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/network/Url.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class PageDto : public oatpp::DTO {
  DTO_INIT(PageDto, DTO)

  DTO_FIELD_INFO(path) { info->description = "Path of page"; }
  DTO_FIELD(String, path);
};

#include OATPP_CODEGEN_END(DTO)

#include OATPP_CODEGEN_BEGIN(ApiController)

class StaticController : public oatpp::web::server::api::ApiController {
 public:
  StaticController(const std::shared_ptr<oatpp::web::mime::ContentMappers>&
                       apiContentMappers)
      : oatpp::web::server::api::ApiController(apiContentMappers) {}

  static std::shared_ptr<StaticController> createShared(
      OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                      apiContentMappers)  // Inject ContentMappers
  ) {
    return std::make_shared<StaticController>(apiContentMappers);
  }

  ENDPOINT("GET", "/*", root,
           REQUEST(std::shared_ptr<IncomingRequest>, request),
           QUERIES(QueryParams, queries)) {
    std::string raw_url = request->getStartingLine().path.std_str();
    auto url = oatpp::network::Url::Parser::parseUrl(raw_url);
    std::string path = url.path;
    if (path.size() > 0) {
      path.erase(0, 1);  // erase leading /
    }

    Status ret_status = Status::CODE_200;

    if (path == "error") {
      ret_status = Status::CODE_300;
    }

    if (path.ends_with("/") || path.empty()) {
      path.append("index.html");
    }

    auto file_path = std::filesystem::current_path() / "public" / path;

    if (file_path.extension() == "") {
      file_path += ".html";
    }

    if (!std::filesystem::exists(file_path)) {
      ret_status = Status::CODE_404;
      file_path = std::filesystem::current_path() / "public" / "404.html";
    }

    auto* file_fd = fopen(file_path.c_str(), "r");
    if (file_fd != nullptr) {
      fseek(file_fd, 0, SEEK_END);
      auto file_size = ftell(file_fd);
      fseek(file_fd, 0, SEEK_SET);
      std::string file_str(file_size, '\0');
      fread(file_str.data(), 1, file_size, file_fd);
      fclose(file_fd);

      auto response = createResponse(ret_status, file_str);

      std::string mime = "text/html";

      if (file_path.extension() == ".css") {
        mime = "text/css";
      } else if (file_path.extension() == ".js") {
        mime = "text/javascript";
      } else if (file_path.extension() == ".png") {
        mime = "image/png";
      } else if (file_path.extension() == ".webp") {
        mime = "image/webp";
      } else if (file_path.extension() == ".html" ||
                 file_path.extension() == "") {
      } else {
        OATPP_LOGw("Static PalatineHill", "Could not find MIME for {}",
                   file_path.extension().string());
      }

      response->putHeader(Header::CONTENT_TYPE, mime);
      return response;
    }
    auto response = createResponse(ret_status, "failed to get content");
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }
};

#include OATPP_CODEGEN_END(ApiController)
