#include "api.h"
#include "component.h"
#include "oatpp/base/Log.hpp"
#include "oatpp/network/Server.hpp"
#include "static.h"

#define PORT 8080

int main(int argv, char* argc[]) {
  oatpp::Environment::init();

  AppComponent components;

  OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

  router->addController(ApiController::createShared());
  router->addController(StaticController::createShared());

  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                  connection_handler);
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>,
                  connection_provider);

  oatpp::network::Server server(connection_provider, connection_handler);

  OATPP_LOGi("PalatineHill", "Server running on port http://127.0.0.1:{}",
             PORT);

  server.run();

  oatpp::Environment::destroy();
  return 0;
}
