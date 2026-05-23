#pragma once
#include <exception>
#include <print>

#include "oatpp/macro/codegen.hpp"
#include "oatpp/orm/DbClient.hpp"
#include "oatpp/orm/SchemaMigration.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

class LocalDb : public oatpp::orm::DbClient {
 public:
  LocalDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
      : oatpp::orm::DbClient(executor) {
    oatpp::orm::SchemaMigration migration(executor);
    migration.addFile(1, "migrations/init.sql");

    try {
      migration.migrate();
    } catch (const std::exception& e) {
      std::println(stderr, "{}", e.what());
    }
  }

  oatpp::data::share::StringTemplate template_CREATOR_register_access_token();

  const oatpp::data::share::StringTemplate template_register_access_token =
      template_CREATOR_register_access_token();

  std::shared_ptr<oatpp::orm::QueryResult> register_access_token(
      const oatpp::String& slack_id, const oatpp::String& access_token,
      const oatpp::String& first_name,
      const oatpp::provider::ResourceHandle<oatpp::orm::Connection>&
          connection = nullptr);
};

#include OATPP_CODEGEN_END(DbClient)
