#pragma once
#include <exception>
#include <print>

#include "oatpp/macro/codegen.hpp"
#include "oatpp/orm/DbClient.hpp"
#include "oatpp/orm/SchemaMigration.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class AccountInfoDto : public oatpp::DTO {
  DTO_INIT(AccountInfoDto, DTO)

  DTO_FIELD_INFO(slack_id) {}
  DTO_FIELD(String, slack_id);

  DTO_FIELD_INFO(access_token) {}
  DTO_FIELD(String, access_token);

  DTO_FIELD_INFO(name) {}
  DTO_FIELD(String, name);
};

#include OATPP_CODEGEN_END(DTO)

#include OATPP_CODEGEN_BEGIN(DTO)

class PostDto : public oatpp::DTO {
  DTO_INIT(PostDto, DTO)

  DTO_FIELD_INFO(name) {}
  DTO_FIELD(String, name);

  DTO_FIELD_INFO(slack_id) {}
  DTO_FIELD(String, slack_id);

  DTO_FIELD_INFO(title) {}
  DTO_FIELD(String, title);

  DTO_FIELD_INFO(explanation) {}
  DTO_FIELD(String, explanation);

  DTO_FIELD_INFO(id) {}
  DTO_FIELD(UInt32, id);

  DTO_FIELD_INFO(pitch_timestamp) {}
  DTO_FIELD(String, pitch_timestamp);
};

#include OATPP_CODEGEN_END(DTO)

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

  QUERY(create_pitch,
        "INSERT INTO pitches (slack_id, title, explanation, pitch_timestamp) "
        "VALUES (:slack_id, "
        ":title, :explanation, :pitch_timestamp)",
        PARAM(oatpp::String, slack_id), PARAM(oatpp::String, title),
        PARAM(oatpp::String, explanation),
        PARAM(oatpp::String, pitch_timestamp))

  QUERY(get_account_from_access_token,
        "SELECT * FROM access_tokens WHERE access_token=:access_token",
        PARAM(oatpp::String, access_token))

  QUERY(get_pitches,
        "SELECT name, pitches.* FROM pitches INNER JOIN access_tokens ON "
        "pitches.slack_id=access_tokens.slack_id")
};

#include OATPP_CODEGEN_END(DbClient)
