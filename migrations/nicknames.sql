CREATE TABLE "nicknames" (
	"slack_id"	TEXT NOT NULL UNIQUE,
	"name"	TEXT NOT NULL,
	"last_updated"	TEXT,
	PRIMARY KEY("slack_id"),
	FOREIGN KEY("slack_id") REFERENCES "access_tokens"("slack_id")
);
