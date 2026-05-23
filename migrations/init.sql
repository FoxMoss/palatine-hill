CREATE TABLE "access_tokens" (
	"slack_id"	TEXT NOT NULL UNIQUE,
	"access_token"	TEXT NOT NULL,
	"name"	TEXT NOT NULL,
	PRIMARY KEY("slack_id")
)
