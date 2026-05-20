
#pragma once

#include "oatpp/Types.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/web/protocol/http/outgoing/ResponseFactory.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class StatusDto : public oatpp::DTO {
  DTO_INIT(StatusDto, DTO)

  DTO_FIELD_INFO(status) { info->description = "Short status text"; }
  DTO_FIELD(String, status);

  DTO_FIELD_INFO(code) { info->description = "Status code"; }
  DTO_FIELD(Int32, code);

  DTO_FIELD_INFO(message) { info->description = "Verbose message"; }
  DTO_FIELD(String, message);
};

#include OATPP_CODEGEN_END(DTO)

class ErrorHandler : public oatpp::web::server::handler::DefaultErrorHandler {
 private:
  using OutgoingResponse = oatpp::web::protocol::http::outgoing::Response;
  using Status = oatpp::web::protocol::http::Status;
  using ResponseFactory = oatpp::web::protocol::http::outgoing::ResponseFactory;

 private:
  std::shared_ptr<oatpp::web::mime::ContentMappers> m_mappers;

 public:
  ErrorHandler(
      const std::shared_ptr<oatpp::web::mime::ContentMappers>& mappers);

  std::shared_ptr<OutgoingResponse> renderError(
      const HttpServerErrorStacktrace& stacktrace) override;
};

