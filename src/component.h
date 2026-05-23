#pragma once

#include <print>

#include "dbclient.h"
#include "error.h"
#include "oatpp-sqlite/ConnectionProvider.hpp"
#include "oatpp-sqlite/Executor.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

#define PORT 8080

class AppComponent {
 public:
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                         api_content_mappers)([] {
    auto json = std::make_shared<oatpp::json::ObjectMapper>();
    json->serializerConfig().json.useBeautifier = true;

    auto mappers = std::make_shared<oatpp::web::mime::ContentMappers>();
    mappers->putMapper(json);

    return mappers;
  }());

  OATPP_CREATE_COMPONENT(
      std::shared_ptr<oatpp::network::ServerConnectionProvider>,
      serverConnectionProvider)([] {
    return oatpp::network::tcp::server::ConnectionProvider::createShared(
        {"0.0.0.0", PORT, oatpp::network::Address::IP_4});
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>,
                         httpRouter)([] {
    return oatpp::web::server::HttpRouter::createShared();
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                         serverConnectionHandler)([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                    contentMappers);

    auto connectionHandler =
        oatpp::web::server::HttpConnectionHandler::createShared(router);
    connectionHandler->setErrorHandler(
        std::make_shared<ErrorHandler>(contentMappers));
    return connectionHandler;
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<LocalDb>, dbclient)([] {
    auto connectionProvider =
        std::make_shared<oatpp::sqlite::ConnectionProvider>("persistent.db");

    auto executor =
        std::make_shared<oatpp::sqlite::Executor>(connectionProvider);

    return std::make_shared<LocalDb>(executor);
  }());
};

