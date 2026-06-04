#include "dbclient.h"

std::shared_ptr<oatpp::orm::QueryResult> LocalDb::register_access_token(
    const oatpp::String& slack_id, const oatpp::String& access_token,
    const oatpp::String& first_name,
    const oatpp::provider::ResourceHandle<oatpp::orm::Connection>& connection) {
  std::unordered_map<oatpp::String, oatpp::Void> params;
  params.insert({"slack_id", slack_id});
  params.insert({"access_token", access_token});
  params.insert({"name", first_name});
  return this->execute(template_register_access_token, params, connection);
}

oatpp::data::share::StringTemplate
LocalDb::template_creator_register_access_token() {
  bool prepare = false;
  oatpp::orm::Executor::ParamsTypeMap map;
  map.insert({"slack_id", oatpp::String::Class::getType()});
  map.insert({"access_token", oatpp::String::Class::getType()});
  map.insert({"name", oatpp::String::Class::getType()});
  return this->parseQueryTemplate(
      "register_access_token",
      "REPLACE INTO access_tokens (slack_id, "
      "access_token, name, last_updated) VALUES "
      "(:slack_id, :access_token, :name, unixepoch());",
      map, prepare);
}

