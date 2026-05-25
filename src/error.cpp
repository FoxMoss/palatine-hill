#include "error.h"

#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/encoding/Url.hpp"
#include <format>

ErrorHandler::ErrorHandler(
    const std::shared_ptr<oatpp::web::mime::ContentMappers> &mappers)
    : m_mappers(mappers) {}

std::shared_ptr<ErrorHandler::OutgoingResponse>
ErrorHandler::renderError(const HttpServerErrorStacktrace &stacktrace) {
  Status status = stacktrace.status;
  if (status.description == nullptr) {
    status.description = "Unknown";
  }

  oatpp::data::stream::BufferOutputStream string_stream;

  for (const auto &stack_frame : stacktrace.stack) {
    string_stream << stack_frame << "\n";
  }

  auto error = StatusDto::createShared();
  error->status = "ERROR";
  error->code = stacktrace.status.code;
  error->message = string_stream.toString();

  std::vector<oatpp::String> accept;
  if (stacktrace.request) {
    accept = stacktrace.request->getHeaderValues("Accept");
  }

  auto mapper = m_mappers->selectMapper(accept);
  if (!mapper) {
    mapper = m_mappers->getDefaultMapper();
  }

  auto response =
      ResponseFactory::createResponse(Status::CODE_303, error, mapper);

  for (const auto &pair : stacktrace.headers.getAll()) {
    response->putHeader(pair.first.toString(), pair.second.toString());
  }

  response->putHeader(
      "Location",
      std::format("/error?error={}",
                  (std::string)oatpp::encoding::Url::encode(
                      std::format("{} {}", stacktrace.status.code,
                                  (std::string)string_stream.toString()),
                      oatpp::encoding::Url::Config())));

  return response;
}
