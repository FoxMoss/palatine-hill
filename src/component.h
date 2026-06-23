#pragma once

#include <memory>
#include <print>

#include "dbclient.h"
#include "env.h"
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
    mappers->setDefaultMapper(json);

    return mappers;
  }());

  OATPP_CREATE_COMPONENT(
      std::shared_ptr<oatpp::network::ServerConnectionProvider>,
      server_connection_provider)([] {
    return oatpp::network::tcp::server::ConnectionProvider::createShared(
        {"0.0.0.0", PORT, oatpp::network::Address::IP_4});
  }());


  OATPP_CREATE_COMPONENT(std::shared_ptr<EnvReader>, env_reader)([] {
    return std::make_shared<EnvReader>(".env.json");
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>,
                         http_router)([] {
    return oatpp::web::server::HttpRouter::createShared();
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                         serverConnectionHandler)([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>,
                    content_mappers);

    auto connection_handler =
        oatpp::web::server::HttpConnectionHandler::createShared(router);
    connection_handler->setErrorHandler(
        std::make_shared<ErrorHandler>(content_mappers));
    return connection_handler;
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<LocalDb>, dbclient)([] {
    auto connection_provider =
        std::make_shared<oatpp::sqlite::ConnectionProvider>("persistent/persistent.db");

    auto executor =
        std::make_shared<oatpp::sqlite::Executor>(connection_provider);

    return std::make_shared<LocalDb>(executor);
  }());
};

